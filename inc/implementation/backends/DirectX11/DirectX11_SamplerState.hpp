#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "interfaces/rendering/SamplerState.hpp"

namespace orbit
{

    class DirectX11SamplerState : public ISamplerState
    {
    private:
        ComPtr<ID3D11SamplerState> m_samplerState;
    public:
        bool LoadImpl(std::ifstream*) override;
        void UnloadImpl() override { m_samplerState = nullptr; }
        virtual void Bind(uint32_t slot) const override;
    };

}

#endif