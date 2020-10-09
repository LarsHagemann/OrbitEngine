#include "engine.hpp"
#include "renderable.hpp"
#include "event_driven.hpp"
#include "exception.hpp"

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
				_swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)),
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

		// only resize if the size has actually changed
		if (_state._dimensions.x() != size.x() || _state._dimensions.y() != size.y())
		{
			_state._dimensions.x() = std::min(1, size.x());
			_state._dimensions.y() = std::min(1, size.y());

			Flush();

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
		}
	}

	void Engine::UpdateAndDraw()
	{
		MSG _message;
		while (PeekMessage(&_message, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&_message);
			DispatchMessage(&_message);
		}

		auto commandAllocator = _commandAllocators[_state._currentBackbuffer];
		commandAllocator->Reset();
		_cmdList->Reset(commandAllocator.Get(), nullptr);

		// go through each object in the scene and call
		// Update(). If the object has a renderable component
		// call Draw();
		auto dt = _state._clock.Restart();
		for (const auto& object : _scene->_objects)
		{
			if (!object.second->IsActive()) continue;

			object.second->Update(dt);
			// go through all components and look for 
			// Renderable and EventDriven components
			for (auto component : object.second->_components)
			{
				auto rndComp = std::dynamic_pointer_cast<Renderable>(component.second);
				auto evntComp = std::dynamic_pointer_cast<EventDriven>(component.second);
				if (rndComp) rndComp->Draw(_cmdList);
				if (evntComp) evntComp->Update(dt);
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
		_cmdList->ResourceBarrier(1, &barrier);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_state._currentBackbuffer,
			_state._RTVDescriptorSize
		);

		_cmdList->ClearRenderTargetView(
			rtv,
			_state._clearColor.data(),
			0,
			nullptr
		);
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
		_cmdList->ResourceBarrier(1, &barrier);

		ORBIT_THROW_IF_FAILED(
			_cmdList->Close(),
			"Failed to close command list."
		);
		ID3D12CommandList* const commandLists[] = {
			_cmdList.Get()
		};
		_cmdQueue->ExecuteCommandLists(
			_countof(commandLists),
			commandLists
		);
		static const auto syncInterval = _state._vSync ? 1 : 0;
		static const auto presentFlags = _state._tearingSupported && !_state._vSync 
			? DXGI_PRESENT_ALLOW_TEARING 
			: 0;

		ORBIT_THROW_IF_FAILED(
			_swapChain->Present(syncInterval, presentFlags),
			"Failed to present the swap chain."
		);

		_state._fences[_state._currentBackbuffer] = Signal();
		_state._currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();

		WaitForFenceValue(_state._fences[_state._currentBackbuffer]);
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
		_commandAllocators.resize(_state._numBackbuffers);
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

		Ptr<IDXGIAdapter4> adapter = desc->adapter;
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

		D3D12_COMMAND_QUEUE_DESC cmdQDesc;
		ZeroMemory(&cmdQDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
		cmdQDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		cmdQDesc.NodeMask = 0;
		cmdQDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		cmdQDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		ORBIT_THROW_IF_FAILED(
			_device->CreateCommandQueue(&cmdQDesc, IID_PPV_ARGS(_cmdQueue.GetAddressOf())),
			"Failed to create the DX12 Command Queue."
		);

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
				_cmdQueue.Get(),
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
			3
		);
		

		UpdateRenderTargetViews();

		for (auto i = 0u; i < _state._numBackbuffers; ++i)
		{
			Ptr<ID3D12CommandAllocator> allocator;
			ORBIT_THROW_IF_FAILED(_device->CreateCommandAllocator(
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				IID_PPV_ARGS(allocator.GetAddressOf())),
				"Failed to create command allocator."
			);
			_commandAllocators[i] = allocator;
		}

		ORBIT_THROW_IF_FAILED(_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			_commandAllocators[0].Get(),
			nullptr,
			IID_PPV_ARGS(_cmdList.GetAddressOf())),
			"Failed to create the command list."
		);
		_cmdList->Close();

		ORBIT_THROW_IF_FAILED(_device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE, 
			IID_PPV_ARGS(_fence.GetAddressOf())),
			"Failed to create the fence."
		);

		std::fill(_state._fences.begin(), _state._fences.end(), 0);
		_state._RTVDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		_state._currentPipelineState = "default";
		_state._fullscreen = desc->isFullscreen;
		_state._nextFenceValue = 0;
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
			_cmdList->SetPipelineState(pState.Get());
		}
	}

	void Engine::SetScene(std::shared_ptr<Scene> scene)
	{
		_scene = scene;
	}

	uint64_t Engine::Signal()
	{
		ORBIT_THROW_IF_FAILED(_cmdQueue->Signal(
				_fence.Get(),
				_state._nextFenceValue
			),
			"Failed to signal the command queue."
		);
		return _state._nextFenceValue++;
	}

	void Engine::WaitForFenceValue(uint64_t value)
	{
		auto eventHandle = CreateEvent(0, false, false, 0);
		if (_fence->GetCompletedValue() < value)
		{
			ORBIT_THROW_IF_FAILED(
				_fence->SetEventOnCompletion(value, eventHandle),
				"Failed to set event on completion."
			);
			::WaitForSingleObject(eventHandle, INFINITE);
		}
		::CloseHandle(eventHandle);
	}

	void Engine::Flush()
	{
		auto target = Signal();
		WaitForFenceValue(target);
	}

	void Engine::Run()
	{
		ORBIT_INFO("Running engine.");
		ShowWindow(_hWnd, SW_SHOW);
		_state._clock.Restart();
		while (_state._open)
		{
			UpdateAndDraw();
			Clear();
			Display();

			//_physxScene->simulate(_dt);
			//_physxScene->fetchResults(true);
		}

		Flush();
	}

	void Engine::SetFullscreen(bool fullscreen)
	{
		if (_state._fullscreen == fullscreen) return;
		_state._fullscreen = fullscreen;

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

}