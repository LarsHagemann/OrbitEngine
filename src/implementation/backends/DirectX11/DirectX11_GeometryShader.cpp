#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_GeometryShader.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/engine/Engine.hpp"

#include <d3dcompiler.h>

namespace orbit
{

    HRESULT DirectX11GeometryShader::CreateShader(const char* binary, size_t binarySize)
    {
        return ENGINE->Device()->CreateGeometryShader(binary, binarySize, nullptr, m_buffer.ReleaseAndGetAddressOf());
    }

    void DirectX11GeometryShader::Bind() const
    {
        ENGINE->Context()->GSSetShader(m_buffer.Get(), nullptr, 0);
    }
    
}
#endif