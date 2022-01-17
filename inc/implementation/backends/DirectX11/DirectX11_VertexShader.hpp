#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "implementation/backends/DirectX11/DirectX11_Shader.hpp"
#include "interfaces/rendering/Shader.hpp"

namespace orbit
{

    class DirectX11VertexShader : public DirectX11Shader<ID3D11VertexShader>
    {
    public:
        HRESULT CreateShader(const char* binary, size_t binarySize) override;
        void Bind() const override;
        DirectX11VertexShader() :
            DirectX11Shader("vs_5_0")
        {}
    };

}
#endif
