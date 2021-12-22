#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "interfaces/rendering/InputLayout.hpp"

namespace orbit
{

    class DirectX11InputLayout : public IInputLayout<ComPtr<ID3D11InputLayout>>
    {
    public:
        bool LoadImpl(std::ifstream*) override;
        void UnloadImpl() override;
        void Bind() const override;
    };

}
#endif