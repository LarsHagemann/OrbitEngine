#include "engine.hpp"
#include "renderable.hpp"
#include "event_driven.hpp"
#include "exception.hpp"
#ifdef ORBIT_WITH_IMGUI
#include "imgui_impl_win32.h"
#include "imgui.h"
#include "imgui_impl_dx12.h"
#endif

/*
ID3D11Texture2D* pBuffer;
ORBIT_THROW_IF_FAILED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
	(void**)&pBuffer
), "Failed to aquire swap chain buffer");

if (!pBuffer)
	ORBIT_LOG_ERROR_THROW("Failed to aquire swap chain buffer");

D3D11_TEXTURE2D_DESC desc;
pBuffer->GetDesc(&desc);

ORBIT_THROW_IF_FAILED(_device->CreateRenderTargetView(pBuffer, NULL,
	_backBufferTarget.ReleaseAndGetAddressOf()
), "Rendertarget creation failed");

pBuffer->Release();
*/

namespace orbit
{
	void Engine::UpdateRenderTargetViews()
	{
		auto rtvDescriptorSize
			= _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);

		for (auto i = 0u; i < _state._numBackbuffers; ++i)
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
		RECT rect;
		GetClientRect(_hWnd, &rect);
		Vector2i size{ rect.right - rect.left, rect.bottom - rect.top };

		_state._dimensions.x() = std::max(1, size.x());
		_state._dimensions.y() = std::max(1, size.y());

		auto aspect = static_cast<float>(size.x()) / size.y();

		// the aspect ratio changed. 
		// recalculatre the projection matrix of the camera
		if (_scene && _scene->GetCamera())
			_scene->GetCamera()->RecalculateProjectionMatrix(svFOV, aspect, sNearZ, sFarZ);

		_commandQueue->Flush();

		for (auto i = 0u; i < _state._numBackbuffers; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			_backbuffers[i].Reset();
			_state._fences[i] = _state._fences[_state._currentBackbuffer];
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
				_state._numBackbuffers,
				_state._dimensions.x(),
				_state._dimensions.y(),
				swapChainDesc.BufferDesc.Format,
				swapChainDesc.Flags),
			"Failed to resize the swap chain's buffers."
		);

		_state._currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
		UpdateRenderTargetViews();
#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_CreateDeviceObjects();
#endif
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
		_scene->PrepareRendering(_commandList);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_state._currentBackbuffer,
			_state._RTVDescriptorSize
		);

		_commandList->OMSetRenderTargets(
			1,
			&rtv,
			FALSE,
			NULL
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
		_state._frametime = _state._clock.Restart();

		if (_state._framerateLimit > 0)
		{
			// enforce framerate limit
			auto fr = 1.f / _state._framerateLimit;
			if (_state._frametime.asSeconds() < fr)
			{
				auto sleep = static_cast<int>((fr - _state._frametime.asSeconds()) * 1000);
				Sleep(sleep);
			}
		}

		for (const auto& object : _scene->_objects)
		{
			if (!object.second->IsActive()) continue;

			object.second->Update(_state._frametime);
			// go through all components and look for 
			// Renderable and EventDriven components
			for (auto component : object.second->_components)
			{
				auto rndComp = std::dynamic_pointer_cast<Renderable>(component.second);
				auto evntComp = std::dynamic_pointer_cast<EventDriven>(component.second);
				if (rndComp) rndComp->Draw(_commandQueue->GetCommandList());
				if (evntComp) evntComp->Update(_state._frametime);
			}
		}
	}

	void Engine::Clear()
	{
		auto backbuffer = _backbuffers[_state._currentBackbuffer];
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backbuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		_commandList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_state._currentBackbuffer,
			_state._RTVDescriptorSize
		);

		_commandList->ClearRenderTargetView(
			rtv,
			_state._clearColor.data(),
			0,
			nullptr
		);

#ifdef ORBIT_WITH_IMGUI
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), _commandList.Get());
#endif

		//_cmdList->ClearDepthStencilView(
		//	dsvHandle,
		//	D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		//	1.f,
		//	0,
		//	0,
		//	nullptr
		//);
	}

	void Engine::Display()
	{
		auto backbuffer = _backbuffers[_state._currentBackbuffer];
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backbuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		_commandList->ResourceBarrier(1, &barrier);

		_state._fences[_state._currentBackbuffer] = _commandQueue->ExecuteCommandList(_commandList);

		static const auto syncInterval = _state._vSync ? 1 : 0;
		static const auto presentFlags = _state._tearingSupported && !_state._vSync 
			? DXGI_PRESENT_ALLOW_TEARING 
			: 0;

		ORBIT_THROW_IF_FAILED(
			_swapChain->Present(syncInterval, presentFlags),
			"Failed to present the swap chain."
		);

		_state._currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
		_commandQueue->WaitForFenceValue(_state._fences[_state._currentBackbuffer]);
	}
	
	Engine::Engine(EngineDesc* desc)
	{
		ORBIT_INFO("Initializing the Window.");
#ifdef _DEBUG
		ORBIT_THROW_IF_FAILED(
			D3D12GetDebugInterface(IID_PPV_ARGS(_debug.GetAddressOf())),
			"Failed to get DX12 debug interface"
		);
		_debug->EnableDebugLayer();
#endif
		_state._numBackbuffers = desc->numBackbuffers;
		_backbuffers.resize(_state._numBackbuffers);
		_state._fences.resize(_state._numBackbuffers);
		_state._clearColor = { 0.3f, 0.4f, 0.7f, 1.f };

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

		_state._dimensions = desc->dimensions;

		RECT rc = { 0, 0, static_cast<LONG>(_state._dimensions.x()), static_cast<LONG>(_state._dimensions.y()) };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

		_hWnd = ::CreateWindowExW(
			0L,
			L"OrbitWindowClass",
			desc->title.c_str(),
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			_state._dimensions.x(),
			_state._dimensions.y(),
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

		ORBIT_INFO("Initializing DirectX 12.");

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
		swapChainDesc.BufferCount = _state._numBackbuffers;
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
			_state._numBackbuffers
		);
		
		_CBVDescriptorHeap = CreateDescriptorHeap(
			_device,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
			1,
			D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
		);

		UpdateRenderTargetViews();

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
			_state._numBackbuffers,
			DXGI_FORMAT_R8G8B8A8_UNORM, 
			_CBVDescriptorHeap.Get(),
			_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_CBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#endif

		ORBIT_INFO("PhysX initialization.");

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

		std::fill(_state._fences.begin(), _state._fences.end(), 0);
		_state._RTVDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_state._currentPipelineState = "default";
		_state._fullscreen = desc->isFullscreen;
		_state._currentBackbuffer = 0;
		_state._open = true;
		_state._active = true;
		_state._minimized = false;
		_state._maximized = false;
		_state._resizing = false;
	}

	Engine::~Engine()
	{
	}

	std::shared_ptr<Engine> Engine::Create(EngineDesc* desc)
	{
		return std::make_shared<Engine>(desc);
	}

	void Engine::BindPipelineState(const std::string& id)
	{
		if (id != _state._currentPipelineState)
		{
			auto pState = GetPipelineState(id);
			if (pState == nullptr)
				return;

			_state._currentPipelineState = id;
			_commandList->SetPipelineState(pState.Get());
		}
	}

	void Engine::SetScene(std::shared_ptr<Scene> scene)
	{
		_scene = scene;
	}

	void Engine::Run()
	{
		ORBIT_INFO("Running engine.");
		ShowWindow(_hWnd, SW_SHOW);
		OnResize();
		_state._resizeNeccessary = false;
		_state._clock.Restart();
		_state._frametime = Time(0);
		while (_state._open)
		{
			UpdateAndDraw();
			Clear();
			Display();

			_pxScene->simulate(_state._frametime.asSeconds());
			_pxScene->fetchResults(true);

			if (_state._resizeNeccessary)
			{
				_state._resizeNeccessary = false;
				OnResize();
			}
		}

		// this flush is making some difficulties :(
		//_commandQueue->Flush();

#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif
	}

	void Engine::SetFullscreen(bool fullscreen)
	{
		if (_state._fullscreen == fullscreen) return;
		_state._fullscreen = fullscreen;

		_commandQueue->Flush();

		if (fullscreen)
		{
			// toggle to fullscreen mode
			::GetWindowRect(_hWnd, &_state._hWndRect);
			
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
				_state._hWndRect.left,
				_state._hWndRect.top,
				_state._hWndRect.right - _state._hWndRect.left,
				_state._hWndRect.bottom - _state._hWndRect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE
			);

			::ShowWindow(_hWnd, SW_NORMAL);
		}
	}

	void Engine::SetFramerateLimit(int limit)
	{
		_state._framerateLimit = limit;
	}

	void Engine::CloseWindow()
	{
		SendMessage(_hWnd, WM_DESTROY, 0, 0);
	}

	PxDefaultAllocator Engine::gAllocator;
	PxDefaultErrorCallback Engine::gErrorCallback;

}