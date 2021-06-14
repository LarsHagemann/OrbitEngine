#include "Engine/Bindings/DX12/DX12Engine.hpp"
#include "Engine/Bindings/DX12/DX12Renderer.hpp"
#include "Engine/Bindings/DX12/DX12Helper.hpp"
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Misc/Logger.hpp"

#include "Eigen/Dense"

#ifdef ORBIT_WITH_IMGUI
#include "imgui.h"
#ifdef ORBIT_WIN 
#include "backends/imgui_impl_win32.h"
#endif
#include "backends/imgui_impl_dx12.h"
#endif

#include <thread>

namespace orbit
{

	using namespace Eigen;

	InitDesc InitDesc::GetDefaultDesc()
	{
		InitDesc desc;
		desc.adapter = GetFavoredAdapter();
		desc.numBackbuffers = 3;
		desc.sampleCount = 1;
		desc.sampleQuality = 0;
		desc.numThreads = 1;
		desc.useWARP = false;
		desc.physicsUpdatesPerSecond = 60;
		return desc;
	}

	Renderer EngineResources::CreateRenderer()
	{
		return Renderer(EmplaceCommandList());
	}

	Renderer EngineResources::GetDefaultRenderer() const
	{
		return Renderer(_commandLists[0]);
	}

	Ptr<ID3D12GraphicsCommandList> EngineResources::EmplaceCommandList()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_currentBackbuffer,
			_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(
			_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			0
		);

		ID3D12DescriptorHeap* heaps[] = {
			_CBVDescriptorHeap.Get()
		};

		Ptr<ID3D12GraphicsCommandList> cmdList;
		cmdList = _directCommandQueue->GetCommandList();
		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmdList->RSSetViewports(1, &_viewport);
		cmdList->RSSetScissorRects(1, &_scissorRect);
		cmdList->OMSetRenderTargets(
			1,
			&rtv,
			FALSE,
			&dsv
		);
		cmdList->SetDescriptorHeaps(_countof(heaps), heaps);
		std::lock_guard<std::mutex> lock(_listMutex);
		
		_commandLists.emplace_back(cmdList);
		++_rendererIndex;
		return cmdList;
	}

	void EngineResources::OnResize()
	{
		ORBIT_INFO_LEVEL(FormatString("Resizing."), 5);

		RECT rect;
		GetClientRect(_hWnd, &rect);
		Vector2i size{ rect.right - rect.left, rect.bottom - rect.top };
		Vector2i dimensions;

		dimensions.x() = std::max(1, size.x());
		dimensions.y() = std::max(1, size.y());

		_viewport = CD3DX12_VIEWPORT{ 0.f, 0.f, static_cast<float>(size.x()), static_cast<float>(size.y()) };

		_directCommandQueue->Flush();

		for (auto i = 0u; i < _numBackbuffers; ++i)
		{
			// Any references to the back buffers must be released
			// before the swap chain can be resized.
			_backbuffers[i].Reset();
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
				dimensions.x(),
				dimensions.y(),
				swapChainDesc.BufferDesc.Format,
				swapChainDesc.Flags),
			"Failed to resize the swap chain's buffers."
		);

		UpdateRenderTargetViews();
		ResizeDepthBuffer();

		_currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_CreateDeviceObjects();
#endif
	}

	EngineResources::EngineResources(HWND hWnd, InitDesc* desc) :
		_numBackbuffers(desc->numBackbuffers),
		_hWnd(hWnd),
		_rendererIndex(0),
		_numThreads(desc->numThreads)
	{
		_backbuffers.resize(_numBackbuffers);
		_fences.resize(_numBackbuffers);
		_vSync = false;

		ORBIT_INFO_LEVEL(FormatString("Initializing DirectX."), 5);
		ORBIT_INFO_LEVEL(FormatString("Running on %d threads.", _numThreads), 5);

		if (std::thread::hardware_concurrency() < _numThreads)
			ORBIT_WARN(FormatString("Hardware support for %d threads (currently targetted: %d).",
				std::thread::hardware_concurrency(),
				_numThreads));

		RECT dimensions;
		GetClientRect(_hWnd, &dimensions);

		auto width = dimensions.right - dimensions.left;
		auto height = dimensions.bottom - dimensions.top;

		if (desc->useWARP)
			desc->adapter = GetWARPAdapter();

#if defined(_DEBUG)
		Ptr<ID3D12Debug> debug;
		ORBIT_THROW_IF_FAILED(
			D3D12GetDebugInterface(IID_PPV_ARGS(debug.GetAddressOf())),
			"Failed to get DX12 debug interface"
		);
		Ptr<ID3D12Debug1> spDebugController1;
		ORBIT_THROW_IF_FAILED(
			debug->QueryInterface(IID_PPV_ARGS(&spDebugController1)),
			"Failed to query ID3D12Debug1.");
		spDebugController1->EnableDebugLayer();
		spDebugController1->SetEnableGPUBasedValidation(true);
#endif
		D3D_FEATURE_LEVEL featureLevels[] = {
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
		};
		auto numFeatureLevels = _countof(featureLevels);

		HRESULT result;
		unsigned i = 0u;
		for (; i < numFeatureLevels; ++i)
		{
			result = D3D12CreateDevice(
				desc->adapter.Get(),
				featureLevels[i],
				IID_PPV_ARGS(_device.GetAddressOf())
			);
			// If we found a valid configuration, we break
			if (result == S_OK)
				break;
		}

		ORBIT_THROW_IF_FAILED(result, "Creating the directx device failed.");
		ORBIT_INFO_LEVEL(FormatString("Device created using Feature Level %s.", TranslateFeatureLevel(featureLevels[i])), 8);

#ifdef _DEBUG

		Ptr<ID3D12DeviceRemovedExtendedDataSettings> pDredSettings;
		auto hr = D3D12GetDebugInterface(IID_PPV_ARGS(&pDredSettings));
		if (FAILED(hr))
			ORBIT_THROW_HR("Failed to create DRED settings.", hr);
		
		// Turn on AutoBreadcrumbs and Page Fault reporting
		pDredSettings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
		pDredSettings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

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

		_directCommandQueue = std::make_shared<CommandQueue>(_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
		_copyCommandQueue = std::make_shared<CommandQueue>(_device, D3D12_COMMAND_LIST_TYPE_COPY);
		_computeCommandQueue = std::make_shared<CommandQueue>(_device, D3D12_COMMAND_LIST_TYPE_COMPUTE);

		Ptr<IDXGIFactory4> dxgiFactory4;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif

		ORBIT_THROW_IF_FAILED(
			CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)),
			"Failed to create the DXGI Factory."
		);

		_tearingSupported = CheckTearingSupport();

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
			_tearingSupported
			? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
			: 0;

		Ptr<IDXGISwapChain1> swapChain;
		ORBIT_THROW_IF_FAILED(
			dxgiFactory4->CreateSwapChainForHwnd(
				_directCommandQueue->GetCommandQueue().Get(),
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
		ResizeDepthBuffer();

		ORBIT_THROW_IF_FAILED(_device->CreateFence(
			0,
			D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(_fence.GetAddressOf())),
			"Failed to create the fence."
		);

		_currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();

#ifdef ORBIT_WITH_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		// Setup Platform/Renderer bindings
#ifdef ORBIT_WIN
		ImGui_ImplWin32_Init(_hWnd);
#endif
#ifdef ORBIT_DX12
		ImGui_ImplDX12_Init(
			_device.Get(),
			_numBackbuffers,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			_CBVDescriptorHeap.Get(),
			_CBVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_CBVDescriptorHeap->GetGPUDescriptorHandleForHeapStart()
		);
#endif

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
#endif

		ORBIT_INFO_LEVEL("DX12 initialized", 11);
	}

	void EngineResources::ResizeDepthBuffer()
	{
		_directCommandQueue->Flush();

		D3D12_CLEAR_VALUE optimizedClearValue = {};
		optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		optimizedClearValue.DepthStencil = { 1.0f, 0 };

		RECT rect;
		GetClientRect(_hWnd, &rect);

		auto width = rect.right - rect.left;
		auto height = rect.bottom - rect.top;

		ORBIT_THROW_IF_FAILED(_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(
				optimizedClearValue.Format,
				width,
				height,
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
		dsv.Format = optimizedClearValue.Format;
		dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsv.Texture2D.MipSlice = 0;
		dsv.Flags = D3D12_DSV_FLAG_NONE;

		_device->CreateDepthStencilView(
			_dsvBuffer.Get(),
			&dsv,
			_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);
	}

	void EngineResources::UpdateRenderTargetViews()
	{
		auto rtvDescriptorSize
			= _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		auto rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);
		_rtvHandle = rtvHandle;
		_dsvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(
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
			rtvHandle.Offset(1, rtvDescriptorSize);
		}

		_currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
	}

	std::shared_ptr<CommandQueue> EngineResources::GetCommandQueue(D3D12_COMMAND_LIST_TYPE type)
	{
		if (type == D3D12_COMMAND_LIST_TYPE_DIRECT)
			return _directCommandQueue;
		else if (type == D3D12_COMMAND_LIST_TYPE_COMPUTE)
			return _computeCommandQueue;
		else if (type == D3D12_COMMAND_LIST_TYPE_COPY)
			return _copyCommandQueue;
		return nullptr;
	}

	void EngineResources::Clear(Vector4f color)
	{
		ORBIT_INFO_LEVEL(FormatString("Current Backbuffer: %d", _currentBackbuffer), 40);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(
			_RTVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			_currentBackbuffer,
			_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
		);

		CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(
			_DSVDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			0,
			0
		);

		assert(_commandLists.size() == 0 && "Expected command list queue to be empty.");
		_commandLists.reserve(_numThreads + 1);
		auto cmdList = EmplaceCommandList();
		_rendererIndex = 1u;

		auto backbuffer = _backbuffers[_currentBackbuffer];
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backbuffer.Get(),
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET
		);
		cmdList->ResourceBarrier(1, &barrier);
		cmdList->ClearRenderTargetView(
			rtv,
			color.data(),
			0,
			nullptr
		);
		cmdList->ClearDepthStencilView(
			dsv,
			D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			1.f,
			0,
			0,
			nullptr
		);

#ifdef ORBIT_WITH_IMGUI
			ImGui_ImplDX12_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
#endif

		ID3D12DescriptorHeap* heaps[] = {
			_CBVDescriptorHeap.Get()
		};

		cmdList->SetDescriptorHeaps(1, heaps);
	}

	void EngineResources::Display()
	{
		auto cmdList = EmplaceCommandList();

#ifdef ORBIT_WITH_IMGUI
		ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());
#endif

		auto backbuffer = _backbuffers[_currentBackbuffer];
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			backbuffer.Get(),
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT
		);
		cmdList->ResourceBarrier(1, &barrier);

		_fences[_currentBackbuffer] = _directCommandQueue->ExecuteCommandLists(_commandLists);
		_commandLists.clear();

		static const auto syncInterval = _vSync ? 1 : 0;
		static const auto presentFlags = _tearingSupported && !_vSync 
			? DXGI_PRESENT_ALLOW_TEARING 
			: 0;

		ORBIT_THROW_IF_FAILED(
			_swapChain->Present(syncInterval, presentFlags),
			"Failed to present the swap chain."
		);

		_currentBackbuffer = _swapChain->GetCurrentBackBufferIndex();
		_directCommandQueue->WaitForFenceValue(_fences[_currentBackbuffer]);
	}

	void EngineResources::InitResources()
	{
#ifdef _DEBUG
		AddVertexShader("orbit/default", "shader/vs_default_d.fxc");
		AddPixelShader("orbit/default", "shader/ps_default_d.fxc");
#else
		AddVertexShader("orbit/default", "shader/vs_default.fxc");
		AddPixelShader("orbit/default", "shader/ps_default.fxc");
#endif
		AddRootSignature("orbit/default", "shader/rs_default.fxo");
		AddPipelineState("orbit/default", "orbit/default", "orbit/default");
	}

	void EngineResources::Cleanup()
	{
		_directCommandQueue->Flush();
		_copyCommandQueue->Flush();
		_computeCommandQueue->Flush();
	}

	EngineResources::~EngineResources()
	{
		if (_device)
		{
			auto hr = _device->GetDeviceRemovedReason();
			if (FAILED(hr))
				ORBIT_ERR_HR("An error occured.", hr);
		}
	}

	fs::path EngineResources::GetEngineFolder()
	{
		static auto appdata_path = pt::GetAppdataPath();
		return appdata_path / "Treelab"_path / "OrbitEngine"_path;
	}

}