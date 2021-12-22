#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "implementation/backends/DirectX11/DirectX11_Shader.hpp"
#include "interfaces/rendering/Shader.hpp"

namespace orbit
{

    class DirectX11HullShader : public DirectX11Shader<ID3D11HullShader>
    {
    public:
        HRESULT CreateShader(const char* binary, size_t binarySize) override;
        void Bind() const override;
        void UnloadImpl() override {}
        DirectX11HullShader() :
            DirectX11Shader("hs_5_0")
        {}
    };

}
#endif