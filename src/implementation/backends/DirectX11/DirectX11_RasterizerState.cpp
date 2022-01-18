#include "implementation/backends/DirectX11/DirectX11_RasterizerState.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    void DirectX11RasterizerState::Bind() const
    {
        ENGINE->Context()->RSSetState(m_state.Get());
    }

    bool DirectX11RasterizerState::LoadImpl(std::ifstream* stream)
    {
        D3D11_RASTERIZER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
        desc.CullMode = D3D11_CULL_BACK;
        desc.FillMode = D3D11_FILL_SOLID;
        desc.FrontCounterClockwise = false;
        desc.DepthBias = false;
        desc.DepthBiasClamp = 0;
        desc.SlopeScaledDepthBias = 0;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = false;
        desc.MultisampleEnable = true;
        desc.AntialiasedLineEnable = false;
        
        CullMode cMode;
        FillMode fMode;
        stream->read((char*)&cMode, sizeof(CullMode));
        stream->read((char*)&fMode, sizeof(FillMode));
        switch (cMode)
        {
        case CullMode::CULL_BACK: desc.CullMode = D3D11_CULL_BACK; break;
        case CullMode::CULL_FRONT: desc.CullMode = D3D11_CULL_FRONT; break;
        case CullMode::CULL_NONE: desc.CullMode = D3D11_CULL_NONE; break;
        }
        switch (fMode)
        {
        case FillMode::FILL_SOLID: desc.FillMode = D3D11_FILL_SOLID; break;
        case FillMode::FILL_WIREFRAME: desc.FillMode = D3D11_FILL_WIREFRAME; break;
        }

        auto hr = ENGINE->Device()->CreateRasterizerState(&desc, m_state.GetAddressOf());
        if (FAILED(hr))
        {
            ORBIT_ERROR_HR(hr, "Failed to create rasterizer state.");
            return false;
        }
        return true;
    }

}