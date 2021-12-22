#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_VertexShader.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/engine/Engine.hpp"

#include <d3dcompiler.h>

namespace orbit
{

    HRESULT DirectX11VertexShader::CreateShader(const char* binary, size_t binarySize)
    {
        return ENGINE->Device()->CreateVertexShader(binary, binarySize, nullptr, m_buffer.ReleaseAndGetAddressOf());
    }

    void DirectX11VertexShader::Bind() const
    {
        ENGINE->Context()->VSSetShader(m_buffer.Get(), nullptr, 0);
    }
    
}
#endif