#include "implementation/backends/DirectX11/DirectX11_SamplerState.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    void DirectX11SamplerState::Bind(uint32_t slot) const
    {
        ENGINE->Context()->PSSetSamplers(slot, 1, m_samplerState.GetAddressOf());
    }

    bool DirectX11SamplerState::LoadImpl(std::ifstream* stream)
    {
        D3D11_SAMPLER_DESC sDesc;
        ZeroMemory(&sDesc, sizeof(D3D11_SAMPLER_DESC));
        sDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        sDesc.MipLODBias = 0.0f;
        sDesc.MaxAnisotropy = 1;
        sDesc.BorderColor[0] = 0;
        sDesc.BorderColor[1] = 0;
        sDesc.BorderColor[2] = 0;
        sDesc.BorderColor[3] = 0;
        sDesc.MinLOD = 0;
        sDesc.MaxLOD = D3D11_FLOAT32_MAX;
        

        return SUCCEEDED(ENGINE->Device()->CreateSamplerState(&sDesc, m_samplerState.GetAddressOf()));
    }

}