#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "interfaces/rendering/BlendState.hpp"

namespace orbit
{

    class DirectX11BlendState : public IBlendState
    {
    private:
        ComPtr<ID3D11BlendState> m_blendState;
    public:
        void Bind() const override;
        bool LoadImpl(std::ifstream*) override;
        void UnloadImpl() override { m_blendState = nullptr; }
    };

}
#endif
