#include "implementation/backends/DirectX11/DirectX11_BlendState.hpp"

#include "implementation/engine/Engine.hpp"

namespace orbit
{

    void DirectX11BlendState::Bind() const
    {
        ENGINE->Context()->OMSetBlendState(m_blendState.Get(), m_blendFactor, m_sampleMask);
    }

    bool DirectX11BlendState::LoadImpl(std::ifstream* stream)
    {
        D3D11_BLEND_DESC bDesc;
        ZeroMemory(&bDesc, sizeof(D3D11_BLEND_DESC));
        bDesc.IndependentBlendEnable = false;
        // Read bool as single byte value
        stream->read((char*)&bDesc.AlphaToCoverageEnable, 1);
        auto& rt = bDesc.RenderTarget[0];
        
        stream->read((char*)&rt.BlendEnable, 1);
        if (rt.BlendEnable)
        {
            IBlendState::LoadImpl(stream);
            rt.RenderTargetWriteMask = m_sampleMask;
            stream->read((char*)&rt.BlendOp, 1);
            stream->read((char*)&rt.BlendOpAlpha, 1);
            stream->read((char*)&rt.SrcBlend, 1);
            stream->read((char*)&rt.SrcBlendAlpha, 1);
            stream->read((char*)&rt.DestBlend, 1);
            stream->read((char*)&rt.DestBlendAlpha, 1);
        }

        return SUCCEEDED(ENGINE->Device()->CreateBlendState(&bDesc, m_blendState.GetAddressOf()));
    }

}