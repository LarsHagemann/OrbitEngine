#pragma once
#include "implementation/misc/RasterizerInfo.hpp"
#include "interfaces/rendering/RasterizerState.hpp"

namespace orbit
{

    class DirectX11RasterizerState : public IRasterizerState
    {
    private:
        ComPtr<ID3D11RasterizerState> m_state;
    public:
        void Bind() const override;
        bool LoadImpl(std::ifstream* stream) override;
        void UnloadImpl() override { m_state = nullptr; }
    };

}