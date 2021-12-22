#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_DomainShader.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/engine/Engine.hpp"

#include <d3dcompiler.h>

namespace orbit
{

    HRESULT DirectX11DomainShader::CreateShader(const char* binary, size_t binarySize)
    {
        return ENGINE->Device()->CreateDomainShader(binary, binarySize, nullptr, m_buffer.ReleaseAndGetAddressOf());
    }

    void DirectX11DomainShader::Bind() const
    {
        ENGINE->Context()->DSSetShader(m_buffer.Get(), nullptr, 0);
    }
    
}
#endif