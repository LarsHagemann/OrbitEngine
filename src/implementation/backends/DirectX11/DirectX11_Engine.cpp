#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_Engine.hpp"
#include "implementation/backends/DirectX11/DirectX11_Renderer.hpp"

#undef new

#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

#include <wrl/wrappers/corewrappers.h>

namespace orbit
{

    std::shared_ptr<DirectX11Engine> DirectX11Engine::sEngine = nullptr;

    void DirectX11Engine::Clear()
    {
        if (m_window->IsResizeNeccessary())
        {
            auto size = m_window->GetDimensions();
            m_window->Resize(size);
            auto scene = GetCurrentScene();
            if (scene && scene->GetCamera())
                scene->GetCamera()->RecalculateProjectionMatrix();
            
            OnResize();
        }

        ComPtr<ID3D11Texture2D> texture;
        m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &texture);

        m_context->ResolveSubresource(
            texture.Get(),
            0,
            m_renderTargetTexture.Get(),
            0,
            DXGI_FORMAT_R8G8B8A8_UNORM
        );

        m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 1);
        m_context->ClearDepthStencilView(
            m_depthStencilView.Get(),
            D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
            1.f,
            0
        );
        m_context->ClearRenderTargetView(m_renderTargetView.Get(), m_clearColor);
        m_context->PSSetSamplers(1, 1, m_depthSampler.GetAddressOf());
        m_context->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }

    void DirectX11Engine::OnResize()
    {
		RECT rect;
		GetClientRect(m_window->GetHandle(), &rect);
		Vector2i size{ rect.right - rect.left, rect.bottom - rect.top };
		Vector2i dimensions;

		dimensions.x() = std::max(1, size.x());
		dimensions.y() = std::max(1, size.y());

        auto viewport = CD3D11_VIEWPORT(0.f, 0.f, static_cast<float>(size.x()), static_cast<float>(size.y()));


		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ORBIT_THROW_IF_FAILED(
			m_swapChain->GetDesc(&swapChainDesc),
			"Failed to access the swap chain's description."
		);

		ImGui_ImplDX11_InvalidateDeviceObjects();

		ORBIT_THROW_IF_FAILED(
			m_swapChain->ResizeBuffers(
				swapChainDesc.BufferCount,
				dimensions.x(),
				dimensions.y(),
				swapChainDesc.BufferDesc.Format,
				swapChainDesc.Flags),
			"Failed to resize the swap chain's buffers."
		);

		ResizeRenderTargetViews();
		ResizeDepthBuffer();

		ImGui_ImplDX11_CreateDeviceObjects();
    }

    void DirectX11Engine::ResizeRenderTargetViews()
    {
        D3D11_TEXTURE2D_DESC rttDesc;
        ZeroMemory(&rttDesc, sizeof(D3D11_TEXTURE2D_DESC));
        rttDesc.Usage = D3D11_USAGE_DEFAULT;
        rttDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
        rttDesc.ArraySize = 1;
        rttDesc.Width = m_window->GetDimensions().x();
        rttDesc.Height = m_window->GetDimensions().y();
        rttDesc.SampleDesc.Count = m_msTarget;
        rttDesc.SampleDesc.Quality = m_msTarget == 1 ? 0 : 1;
        rttDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rttDesc.MipLevels = 1;

        ORBIT_THROW_IF_FAILED(
            m_device->CreateTexture2D(&rttDesc, nullptr, m_renderTargetTexture.ReleaseAndGetAddressOf()),
            "Failed to create render target texture"
        );

        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

        ORBIT_THROW_IF_FAILED(
            m_device->CreateRenderTargetView(m_renderTargetTexture.Get(), &rtvDesc, m_renderTargetView.ReleaseAndGetAddressOf()),
            "Failed to create render target view"
        );

        D3D11_VIEWPORT viewport;
        viewport.Width = m_window->GetDimensions().x();
        viewport.Height = m_window->GetDimensions().y();
        viewport.MinDepth = 0.f;
        viewport.MaxDepth = 1.f;
        viewport.TopLeftX = 0.f;
        viewport.TopLeftY = 0.f;
        m_context->RSSetViewports(1, &viewport);
    }

    void DirectX11Engine::ResizeDepthBuffer()
    {
        D3D11_TEXTURE2D_DESC depthTexDesc;
        ZeroMemory(&depthTexDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthTexDesc.Width = m_window->GetDimensions().x();
        depthTexDesc.Height = m_window->GetDimensions().y();
        depthTexDesc.MipLevels = 1;
        depthTexDesc.ArraySize = 1;
        depthTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthTexDesc.SampleDesc.Count = m_msTarget;
        depthTexDesc.SampleDesc.Quality = m_msTarget == 1 ? 0 : 1;
        depthTexDesc.CPUAccessFlags = 0;
        depthTexDesc.MiscFlags = 0;
        depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        ORBIT_THROW_IF_FAILED(
            m_device->CreateTexture2D(&depthTexDesc, nullptr, m_depthTexture.ReleaseAndGetAddressOf()), 
            "Failed to create the depth texture"
        );

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
        dsvDesc.Texture2D.MipSlice = 0;
        dsvDesc.Flags = 0;

        ORBIT_THROW_IF_FAILED(
            m_device->CreateDepthStencilView(m_depthTexture.Get(), &dsvDesc, m_depthStencilView.ReleaseAndGetAddressOf()),
            "Failed to create the depth stencil view"
        );
    }

    void DirectX11Engine::Display()
    {
        m_window->HandleEvents();
        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        m_swapChain->Present(
            IsVsynced() ? 1 : 0, 
            0
       );
    }

    void DirectX11Engine::Init(std::shared_ptr<IWindowBase<HWND>> window, DirectX11Engine_Desc desc)
    {
        ORBIT_INFO_LEVEL(ORBIT_LEVEL_MISC, "Initializing DirectX 11");
        m_msTarget = desc.msaa;
        m_window = window;

//#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
//        Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
//        if (FAILED(initialize))
//            ORBIT_THROW_HR(initialize, "Failed to initialize WRL");
//#else
        HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
        if (FAILED(hr))
            ORBIT_THROW_HR(hr, "Failed to initialize WRL");
//#endif

        if (desc.numThreads > std::thread::hardware_concurrency())
        {
            ORBIT_ERROR(
                "Targetted number of threads is larger than hardware support: %d > %d", 
                desc.numThreads, 
                std::thread::hardware_concurrency()
            );
        }
        m_numThreads = std::min(desc.numThreads, std::thread::hardware_concurrency());
        m_renderer = std::make_shared<DirectX11Renderer>();

        DXGI_SWAP_CHAIN_DESC scDesc;
        ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        scDesc.BufferCount = 2;
        scDesc.BufferDesc.Width = window->GetDimensions().x();
        scDesc.BufferDesc.Height = window->GetDimensions().y();
        scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferDesc.RefreshRate.Denominator = 1;
        scDesc.BufferDesc.RefreshRate.Numerator = 60;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.OutputWindow = window->GetHandle();
        scDesc.Windowed = true;
        scDesc.SampleDesc.Count = 1;
        scDesc.SampleDesc.Quality = 0;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        D3D_FEATURE_LEVEL targetFeatureLevels[] = {
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
        };

        auto creationFlags = 0U;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverType = (D3D_DRIVER_TYPE)-1;
        for (auto driver : { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE })
        {
            if (SUCCEEDED(D3D11CreateDeviceAndSwapChain(
                nullptr,
                driver,
                nullptr,
                creationFlags,
                targetFeatureLevels,
                ARRAYSIZE(targetFeatureLevels),
                D3D11_SDK_VERSION,
                &scDesc,
                m_swapChain.ReleaseAndGetAddressOf(),
                m_device.ReleaseAndGetAddressOf(),
                &m_targetedFeatureLevel,
                m_context.ReleaseAndGetAddressOf())))
            {
                driverType = driver;
                break;
            }
        }
        if (driverType == -1)
            ORBIT_THROW("Unable to create DirectX 11 Device");

        ResizeRenderTargetViews();
        ResizeDepthBuffer();

        D3D11_SAMPLER_DESC depthSampler;
        ZeroMemory(&depthSampler, sizeof(D3D11_SAMPLER_DESC));
        depthSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        depthSampler.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        depthSampler.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        depthSampler.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        depthSampler.MipLODBias = 0.0f;
        depthSampler.MaxAnisotropy = 1;
        depthSampler.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        depthSampler.BorderColor[0] = 0;
        depthSampler.BorderColor[1] = 0;
        depthSampler.BorderColor[2] = 0;
        depthSampler.BorderColor[3] = 0;
        depthSampler.MinLOD = 0;
        depthSampler.MaxLOD = D3D11_FLOAT32_MAX;

        ORBIT_THROW_IF_FAILED(
            m_device->CreateSamplerState(&depthSampler, m_depthSampler.ReleaseAndGetAddressOf()),
            "Failed to create depth sampler"
        );

        D3D11_DEPTH_STENCIL_DESC dsDesc;
        ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        dsDesc.DepthEnable = true;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        dsDesc.StencilEnable = true;
        dsDesc.StencilReadMask = 0xFF;
        dsDesc.StencilWriteMask = 0xFF;
        dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        ORBIT_THROW_IF_FAILED(
            m_device->CreateDepthStencilState(&dsDesc, m_depthStencilState.ReleaseAndGetAddressOf()),
            "Failed to create depth stencil state"
        );
        m_context->OMSetDepthStencilState(m_depthStencilState.Get(), 0);

        auto hInstance = GetModuleHandle(nullptr);

		ORBIT_THROW_IF_FAILED(DirectInput8Create(
			hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&m_directInput,
			0
		), "Failed to create directX input device");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer bindings
        ImGui_ImplWin32_Init(m_window->GetHandle());
        ImGui_ImplDX11_Init(
            m_device.Get(),
            m_context.Get()
        );

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        RMInit();
    }

    std::shared_ptr<DirectX11Engine> DirectX11Engine::Get()
    {
        if (sEngine == nullptr)
            sEngine = std::make_shared<DirectX11Engine>();
        
        return sEngine;
    }

    void DirectX11Engine::Shutdown()
    {
        sEngine = nullptr;
    }

    void DirectX11Engine::SetClearColor(uint8_t red, uint8_t green, uint8_t blue)
    {
        m_clearColor[0] = red / 255.f;
        m_clearColor[1] = green / 255.f;
        m_clearColor[2] = blue / 255.f;
    }

    std::vector<uint32_t> DirectX11Engine::GetMSAALevels() const
    {
        std::vector<uint32_t> levels;
        
        constexpr auto mstshift = 4u;
        constexpr auto multiSamplingTarget = 1u << mstshift;

        uint32_t quality = 0u;
        uint32_t target = 0u;

        for (auto i = 0u; i < mstshift; ++i)
        {
            target = multiSamplingTarget >> i;
            const auto result = m_device->CheckMultisampleQualityLevels(
                    DXGI_FORMAT_R8G8B8A8_UNORM,
                    target,
                    &quality
            );
            if (SUCCEEDED(result) && quality != 0)
                levels.emplace_back(target);
        }
        return levels;
    }

}
#endif