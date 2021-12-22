#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "implementation/misc/Logger.hpp"
#include "interfaces/engine/EngineBase.hpp"
#include "interfaces/rendering/WindowBase.hpp"

#include <memory>
#include <limits>
#include <thread>

#include <dinput.h>

namespace orbit
{

    struct DirectX11Engine_Desc
    {
        uint32_t msaa = 1u;
        uint32_t numThreads = 1u;
    };

    class DirectX11Engine : public IEngineBase
    {
    private:
        static std::shared_ptr<DirectX11Engine> sEngine;
    private:
        ComPtr<ID3D11Device> m_device;
        ComPtr<ID3D11DeviceContext> m_context;
        ComPtr<IDXGISwapChain> m_swapChain;
        ComPtr<ID3D11RenderTargetView> m_renderTargetView;
        ComPtr<ID3D11Texture2D> m_renderTargetTexture;
        ComPtr<ID3D11Texture2D> m_depthTexture;
        ComPtr<ID3D11DepthStencilView> m_depthStencilView;
        ComPtr<ID3D11DepthStencilState> m_depthStencilState;
        ComPtr<ID3D11SamplerState> m_depthSampler;
        ComPtr<IDirectInput8> m_directInput;

        std::shared_ptr<IWindowBase<HWND>> m_window;
        D3D_FEATURE_LEVEL m_targetedFeatureLevel;

        uint32_t m_msTarget;

        float m_clearColor[4] = { .25f, .35f, .5f, 1.f };
    private:
        void Clear() override;
        void Display() override;
        bool IsRunning() override { return m_window->IsOpen(); }
        void OnResize() override;
        void ResizeRenderTargetViews();
        void ResizeDepthBuffer();
    public:
        void Init(std::shared_ptr<IWindowBase<HWND>> window, DirectX11Engine_Desc desc);

        static std::shared_ptr<DirectX11Engine> Get();
        static void Shutdown();

        void SetClearColor(uint8_t red, uint8_t green, uint8_t blue);

        std::vector<uint32_t> GetMSAALevels() const;

        std::shared_ptr<IWindowBase<HWND>> Window() { return m_window; }
        ComPtr<ID3D11Device> Device() { return m_device; }
        ComPtr<ID3D11DeviceContext> Context() { return m_context; }
        ComPtr<IDirectInput8> DirectInput() { return m_directInput; }


    };

}
#endif