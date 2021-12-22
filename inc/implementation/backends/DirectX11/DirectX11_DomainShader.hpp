#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "implementation/backends/DirectX11/DirectX11_Shader.hpp"
#include "interfaces/rendering/Shader.hpp"

namespace orbit
{

    class DirectX11DomainShader : public DirectX11Shader<ID3D11DomainShader>
    {
    public:
        HRESULT CreateShader(const char* binary, size_t binarySize) override;
        void Bind() const override;
        void UnloadImpl() override {}
        DirectX11DomainShader() :
            DirectX11Shader("ds_5_0")
        {}
    };

}
#endif