#include "engine.hpp"
#include "renderable.hpp"
#include "event_driven.hpp"
#include "exception.hpp"
#include "helper.hpp"
#include "input_layout.hpp"
#ifdef ORBIT_WITH_IMGUI
#include "imgui_impl_win32.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#endif

#include <string>
#include <ShlObj.h>
#include <d3dx12.h>

namespace orbit
{
	void Engine::UpdateRenderTargetViews()
	{
		auto rtvDescriptorSize
			= _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);
		CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(
			_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);

		for (auto i = 0u; i < _numBackbuffers; ++i)
		{
			Ptr<ID3D12Resource1> backBuffer;
			ORBIT_THROW_IF_FAILED(
				_swapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf())),
				"Failed to access swap chain backbuffer"
			);

			_device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

			_backbuffers[i] = backBuffer;
			rtvHandle.Offset(rtvDescriptorSize);
		}
	}

	void Engine::OnResize()
	{
		ORBIT_INFO_LEVEL(FormatString("Resizing."), 5);

		RECT rect;
		GetClientRect(_hWnd, &rect);
		Vector2i size{ rect.right - rect.left, rect.bottom - rect.top };

		_dimensions.x() = std::max(1, size.x());
		_dimensions.y() = std::max(1, size.y());

		_viewport = CD3DX12_VIEWPORT{ 0.f, 0.f, static_cast<float>(size.x()), static_cast<float>(size.y()) };

		auto aspect = static_cast<float>(size.x()) / size.y();

		// the aspect ratio changed. 
		// recalculatre the projection matrix of the camera
		if (_scene && _scene->GetCamera())
			_scene->GetCamera()->RecalculateProjectionMatrix(svFOV, aspect, sNearZ, sFarZ);

		_commandQueue->Flush();

		for (auto i = 0u; i < _numBackbuffers; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			_backbuffers[i].Reset();
			_fences[i] = _fences[_currentBackbuffer];
		}

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ORBIT_THROW_IF_FAILED(
			_swapChain->GetDesc(&swapChainDesc),
			"Failed to access the swap chain's description."
		);

#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_InvalidateDeviceObjects();
#endif

		ORBIT_THROW_IF_FAILED(
			_swapChain->ResizeBuffers(
				_numBackbuffers,
				_dimensions.x(),
				_dimensions.y(),
				swapChainDesc.BufferDesc.Format,
				swapChainDesc.Flags),
			"Failed to resize the swap chain's buffers."
		);

		_currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
		UpdateRenderTargetViews();
		ResizeDepthBuffers();
#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_CreateDeviceObjects();
#endif
	}

	void Engine::ResizeDepthBuffers()
	{
		_commandQueue->Flush();

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		ORBIT_THROW_IF_FAILED(_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(
				DXGI_FORMAT_D32_FLOAT, 
				_dimensions.x(), 
				_dimensions.y(),
				1,
				0, 
				1, 
				0,
				D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
			),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&optimizedClearValue,
			IID_PPV_ARGS(_dsvBuffer.GetAddressOf())),
			"Failed to create the depth buffer."
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
		dsv.Format = DXGI_FORMAT_D32_FLOAT;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		_device->CreateDepthStencilView(
			_dsvBuffer.Get(), 
			&dsv,
			_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);
	}

	void Engine::UpdateAndDraw()
	{
		MSG _message;
		while (PeekMessage(&_message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&_message);
			DispatchMessage(&_message);
		}

		// get command list here
		_commandList = _commandQueue->GetCommandList();
		_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		_commandList->RSSetViewports(1, &_viewport);
		_commandList->RSSetScissorRects(1, &_scissorRect);
		_scene->PrepareRendering(_commandList);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_currentBackbuffer,
			_RTVDescriptorSize
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(
			_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			0
		);

		_commandList->OMSetRenderTargets(
			1,
			&rtv,
			FALSE,
			&dsv
		);
		ID3D12DescriptorHeap* heaps[] = {
			_CBVDescriptorHeap.Get()
		};

		_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
#endif

		// go through each object in the scene and call
		// Update(). If the object has a renderable component
		// call Draw();
		_frametime = _clock.Restart();

		if (_framerateLimit > 0)
		{
			// enforce framerate limit
			auto fr = 1.f / _framerateLimit;
			if (_frametime.asSeconds() < fr)
			{
				auto sleep = static_cast<int>((fr - _frametime.asSeconds()) * 1000);
				Sleep(sleep);
			}
		}

		for (const auto& object : _scene->_objects)
		{
			if (!object.second->IsActive()) continue;

			object.second->Update(_frametime);
			// go through all components and look for 
			// Renderable and EventDriven components
			for (auto component : object.second->_components)
			{
				auto rndComp = std::dynamic_pointer_cast<Renderable>(component.second);
				auto evntComp = std::dynamic_pointer_cast<EventDriven>(component.second);
				if (rndComp) rndComp->Draw(_commandQueue->GetCommandList());
				if (evntComp) evntComp->Update(_frametime);
			}
		}
	}

	void Engine::Clear()
	{
		auto backbuffer = _backbuffers[_currentBackbuffer];
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backbuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		_commandList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_currentBackbuffer,
			_RTVDescriptorSize
		);

		_commandList->ClearRenderTargetView(
			rtv,
			_clearColor.data(),
			0,
			nullptr
		);

#ifdef ORBIT_WITH_IMGUI
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());
#endif
	}

	void Engine::Display()
	{
		auto backbuffer = _backbuffers[_currentBackbuffer];
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backbuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		_commandList->ResourceBarrier(1, &barrier);

		_fences[_currentBackbuffer] = _commandQueue->ExecuteCommandList(_commandList);

		static const auto syncInterval = _vSync ? 1 : 0;
		static const auto presentFlags = _tearingSupported && !_vSync 
			? DXGI_PRESENT_ALLOW_TEARING 
			: 0;

		ORBIT_THROW_IF_FAILED(
			_swapChain->Present(syncInterval, presentFlags),
			"Failed to present the swap chain."
		);

		_currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
		_commandQueue->WaitForFenceValue(_fences[_currentBackbuffer]);
	}
	
	Engine::Engine(EngineDesc* desc, const std::wstring& project_name)
	{
		ORBIT_INFO_LEVEL(FormatString("Initializing window."), 5);
#ifdef _DEBUG
		ORBIT_THROW_IF_FAILED(
			D3D12GetDebugInterface(IID_PPV_ARGS(_debug.GetAddressOf())),
			"Failed to get DX12 debug interface"
		);
		Ptr<ID3D12Debug1> spDebugController1;
		ORBIT_THROW_IF_FAILED(
			_debug->QueryInterface(IID_PPV_ARGS(&spDebugController1)),
			"Failed to query ID3D12Debug1.");
		spDebugController1->EnableDebugLayer();
		spDebugController1->SetEnableGPUBasedValidation(true);
		//spDebugController1->SetEnableSynchronizedCommandQueueValidation(true);
#endif
		_projectName = project_name;
		auto enginePath = GetEngineFolder();
		auto projectPath = enginePath / _projectName;
		fs::current_path(projectPath);
		if (!fs::exists(projectPath))
		{
			fs::create_directories(projectPath);
			fs::create_directory(projectPath / "shader");
			fs::create_directory(projectPath / "assets");
			fs::copy(enginePath / "__general"_path / "shader"_path, projectPath / "shader"_path);
		}

		_numBackbuffers = desc->numBackbuffers;
		_backbuffers.resize(_numBackbuffers);
		_fences.resize(_numBackbuffers);
		_sampleCount = desc->sampleCount;
		_sampleQuality = desc->sampleQuality;

		WNDCLASSEX wndClass;
		ZeroMemory(&wndClass, sizeof(WNDCLASSEX));
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.lpfnWndProc = static_window_callback;
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hInstance = NULL;
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszClassName = NULL;
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpszClassName = L"OrbitWindowClass";

		ORBIT_THROW_IF_FAILED(
			RegisterClassExW(&wndClass),
			"Failed to register the window class"
		);

		_dimensions = desc->dimensions;

		RECT rc = { 0, 0, static_cast<LONG>(_dimensions.x()), static_cast<LONG>(_dimensions.y()) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		_hWnd = ::CreateWindowExW(
			0L,
			L"OrbitWindowClass",
			desc->title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			_dimensions.x(),
			_dimensions.y(),
			nullptr,
			nullptr,
			nullptr,
			this
		);

		if (!_hWnd)
		{
			auto le = GetLastError();
			ORBIT_ERR(FormatString(
				"GetLastError: %d; %s\n",
				le,
				std::system_category().message(le).c_str()
				)
			);
			ORBIT_THROW("Failed to create the render window");
		}

		ORBIT_INFO_LEVEL(FormatString("Initializing DirectX."), 5);

		RECT dimensions;
		GetClientRect(_hWnd, &dimensions);

		auto width = dimensions.right - dimensions.left;
		auto height = dimensions.bottom - dimensions.top;

		Ptr<IDXGIAdapter> adapter = desc->adapter;
		if (desc->useWARP)
			adapter = GetWARPAdapter();

		ORBIT_THROW_IF_FAILED(D3D12CreateDevice(
			adapter.Get(),
			D3D_FEATURE_LEVEL_11_0,
			IID_PPV_ARGS(_device.GetAddressOf())
		), "Creating the directx device failed.");

#if defined(_DEBUG)
		Ptr<ID3D12InfoQueue> pInfoQueue;
		if (SUCCEEDED(_device.As(&pInfoQueue)))
		{
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
			pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

			D3D12_MESSAGE_SEVERITY Severities[] =
			{
				D3D12_MESSAGE_SEVERITY_INFO
			};

			D3D12_MESSAGE_ID DenyIds[] = {
				//D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
				D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                      
				D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                    
			};

			D3D12_INFO_QUEUE_FILTER NewFilter = {};
			NewFilter.DenyList.NumSeverities = _countof(Severities);
			NewFilter.DenyList.pSeverityList = Severities;
			NewFilter.DenyList.NumIDs = _countof(DenyIds);
			NewFilter.DenyList.pIDList = DenyIds;

			ORBIT_THROW_IF_FAILED(
				pInfoQueue->PushStorageFilter(&NewFilter),
				"Failed to push Storage Filter."
			);
		}
#endif
		_commandQueue = std::make_shared<CommandQueue>(_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		_copyCommandQueue = std::make_shared<CommandQueue>(_device, D3D12_COMMAND_LIST_TYPE_COPY);

		Ptr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ORBIT_THROW_IF_FAILED(
			CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)),
			"Failed to create the DXGI Factory."
		);

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
		swapChainDesc.Width = width;
		swapChainDesc.Height = height;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = desc->sampleCount;
		swapChainDesc.SampleDesc.Quality = desc->sampleQuality;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = _numBackbuffers;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Flags = 
			CheckTearingSupport()
			? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
			: 0;

		Ptr<IDXGISwapChain1> swapChain;
		ORBIT_THROW_IF_FAILED(
			dxgiFactory4->CreateSwapChainForHwnd(
				_commandQueue->GetCommandQueue().Get(),
				_hWnd,
				&swapChainDesc,
				nullptr,
				nullptr,
				swapChain.GetAddressOf()
			),
			"Failed to create the swap chain."
		);
		ORBIT_THROW_IF_FAILED(
			dxgiFactory4->MakeWindowAssociation(_hWnd, DXGI_MWA_NO_ALT_ENTER),
			"Failed to make window association."
		);
		ORBIT_THROW_IF_FAILED(
			swapChain.As(&_swapChain),
			"Failed to create the swap chain."
		);

		_RTVDescriptorHeap = CreateDescriptorHeap(
			_device,
			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			_numBackbuffers
		);
		
		_CBVDescriptorHeap = CreateDescriptorHeap(
			_device,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		);

		_DSVDescriptorHeap = CreateDescriptorHeap(
			_device,
			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			_numBackbuffers
		);

		UpdateRenderTargetViews();
		ResizeDepthBuffers();

		ORBIT_THROW_IF_FAILED(_device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE, 
			IID_PPV_ARGS(_fence.GetAddressOf())),
			"Failed to create the fence."
		);

		auto hInstance = GetModuleHandle(nullptr);

		ORBIT_THROW_IF_FAILED(DirectInput8Create(
			hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&_directInput,
			0
		), "Failed to create directX input device");

#ifdef ORBIT_WITH_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
		ImGui_ImplWin32_Init(_hWnd);
		ImGui_ImplDX12_Init(
			_device.Get(), 
			_numBackbuffers,
			DXGI_FORMAT_R8G8B8A8_UNORM, 
			_CBVDescriptorHeap.Get(),
			_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_CBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#endif

		ORBIT_INFO_LEVEL(FormatString("Initializing NVidia PhysX."), 5);

		_pxFoundation = PxCreateFoundation(
			PX_PHYSICS_VERSION,
			gAllocator,
			gErrorCallback
		);
		if (_pxFoundation == nullptr)
			ORBIT_THROW("Failed to create PhysX foundation.");

		auto scale = PxTolerancesScale();
		scale.speed = 4.f;
		_pxPvd = PxCreatePvd(*_pxFoundation);
		_pxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *_pxFoundation, scale);
		_pxCooking = PxCreateCooking(PX_PHYSICS_VERSION, *_pxFoundation, PxCookingParams(scale));
		if (_pxFoundation == nullptr || _pxPvd == nullptr || _pxCooking == nullptr)
			ORBIT_THROW("Failed to load NVidia Physx.");

#ifdef _DEBUG
		auto PVD_HOST = "127.0.0.1";
		auto transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		if (!_pxPvd->connect(*transport, PxPvdInstrumentationFlag::eALL))
			ORBIT_INFO("Visual debugger not connected");
		else
			ORBIT_INFO("Visual debugger connected.");
#endif

		_pxPhysics = PxCreatePhysics(
			PX_PHYSICS_VERSION,
			*_pxFoundation,
			scale,
			true,
			_pxPvd
		);

		if (_pxPhysics == nullptr)
			ORBIT_THROW("Failed to load NVidia Physx.");

		PxSceneDesc sceneDesc(scale);
		sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		sceneDesc.gravity = PxVec3(0.f, 0.f, 0.f);
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;
		sceneDesc.bounceThresholdVelocity = 1.9f;

		_pxScene = _pxPhysics->createScene(sceneDesc);
		if (_pxScene == nullptr)
			ORBIT_THROW("Failed to load NVidia Physx Scene.");

		_pxControllerManager = PxCreateControllerManager(*_pxScene);
		if (_pxControllerManager == nullptr)
			ORBIT_THROW("Failed to load NVidia Physx controller manager.");

		ORBIT_INFO_LEVEL(FormatString("Loading default resources."), 5);

#ifdef _DEBUG
		AddVertexShader("orbit/default", "shader/vs-d.fxc");
		AddPixelShader("orbit/default", "shader/ps-d.fxc");
#else
		AddVertexShader("orbit/default", "shader/vs.fxc");
		AddPixelShader("orbit/default", "shader/ps.fxc");
#endif
		AddRootSignature("orbit/default", "shader/rs.fxo");
		AddPipelineState("orbit/default", "orbit/default", "orbit/default", "orbit/default");

		std::fill(_fences.begin(), _fences.end(), 0);
		_RTVDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_currentPipelineState = "default";
		_fullscreen = desc->isFullscreen;
		_clearColor = desc->clearColor;
		_currentBackbuffer = 0;
		_open = true;
		_active = true;
		_minimized = false;
		_maximized = false;
		_resizing = false;
	}

	Engine::~Engine()
	{
	}

	void Engine::BindTexture(unsigned texSlot, const std::string& textureId, Ptr<ID3D12GraphicsCommandList> cmdList)
	{
		if (texSlot >= 4 || _boundTextures[texSlot] == textureId) return;

		std::shared_ptr<Texture> texture = GetTexture(textureId);
		if (!texture) return;

		_boundTextures[texSlot] = textureId;

		D3D12_GPU_DESCRIPTOR_HANDLE texHandle;
		texHandle.ptr = texture->GetResource()->GetGPUVirtualAddress();
		CD3DX12_GPU_DESCRIPTOR_HANDLE ctexHandle;
		ctexHandle.InitOffsetted(texHandle, texSlot);

		cmdList->SetGraphicsRootDescriptorTable(1, ctexHandle);
	}

	std::shared_ptr<Engine> Engine::Create(EngineDesc* desc, const std::wstring& project_name)
	{
		return std::make_shared<Engine>(desc, project_name);
	}

	std::shared_ptr<CommandQueue> Engine::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		if (type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			return _commandQueue;
		else if (type == D3D12_COMMAND_LIST_TYPE_COPY)
			return _copyCommandQueue;

		return nullptr;
	}

	void Engine::BindPipelineState(const std::string& id)
	{
		if (id != _currentPipelineState )
		{
			auto pState = GetPipelineState(id);
			if (pState == nullptr)
				return;

			_currentPipelineState = id;
			_commandList->SetPipelineState(pState.Get());
		}
	}

	void Engine::SetScene(std::shared_ptr<Scene> scene)
	{
		_scene = scene;
	}

	void Engine::Run()
	{
		ORBIT_INFO_LEVEL(FormatString("Running engine."), 5);
		ShowWindow(_hWnd, SW_SHOW);
		OnResize();
		_resizeNeccessary = false;
		_clock.Restart();
		_frametime = Time(0);
		while (_open)
		{
			UpdateAndDraw();
			Clear();
			Display();

			_pxScene->simulate(_frametime.asSeconds());
			_pxScene->fetchResults(true);

			if (_resizeNeccessary)
			{
				_resizeNeccessary = false;
				OnResize();
			}

			ORBIT_INFO_LEVEL(FormatString("Next Frame"), 11);
		}

		// this flush is making some difficulties :(
		_commandQueue->Flush();
		_copyCommandQueue->Flush();

		ORBIT_INFO_LEVEL(FormatString("Shutting down."), 5);

#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif
	}

	void Engine::SetFullscreen(bool fullscreen)
	{
		if (_fullscreen == fullscreen) return;
		_fullscreen = fullscreen;

		ORBIT_INFO_LEVEL(FormatString("Toggling fullscreen mode."), 8);

		_commandQueue->Flush();

		if (fullscreen)
		{
			// toggle to fullscreen mode
			::GetWindowRect(_hWnd, &_hWndRect);
			
			UINT windowStyle = WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			::SetWindowLongW(_hWnd, GWL_STYLE, windowStyle);

			HMONITOR hMonitor = ::MonitorFromWindow(_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitorInfo = {};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			::GetMonitorInfo(hMonitor, &monitorInfo);

			::SetWindowPos(
				_hWnd, 
				HWND_TOP,
				monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.top,
				monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left,
				monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			::ShowWindow(_hWnd, SW_MAXIMIZE);
		}
		else
		{
			// recover from fullscreen mode
			::SetWindowLong(_hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);

			::SetWindowPos(
				_hWnd, 
				HWND_NOTOPMOST,
				_hWndRect.left,
				_hWndRect.top,
				_hWndRect.right - _hWndRect.left,
				_hWndRect.bottom - _hWndRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			::ShowWindow(_hWnd, SW_NORMAL);
		}
	}

	void Engine::SetFramerateLimit(int limit)
	{
		_framerateLimit = limit;
	}

	void Engine::CloseWindow()
	{
		SendMessage(_hWnd, WM_DESTROY, 0, 0);
	}

	fs::path Engine::GetEngineFolder()
	{
		PWSTR buffer;
		SHGetKnownFolderPath(
			FOLDERID_RoamingAppData,
			0,
			0,
			&buffer
		);
		return buffer / "Treelab"_path / "OrbitEngine"_path;
	}

	fs::path Engine::GetProjectFolder() const
	{
		return GetEngineFolder() / _projectName;
	}

	PxDefaultAllocator Engine::gAllocator;
	PxDefaultErrorCallback Engine::gErrorCallback;

}