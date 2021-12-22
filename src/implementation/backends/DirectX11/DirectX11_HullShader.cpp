#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_HullShader.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/engine/Engine.hpp"

#include <d3dcompiler.h>

namespace orbit
{

    HRESULT DirectX11HullShader::CreateShader(const char* binary, size_t binarySize)
    {
        return ENGINE->Device()->CreateHullShader(binary, binarySize, nullptr, m_buffer.ReleaseAndGetAddressOf());
    }

    void DirectX11HullShader::Bind() const
    {
        ENGINE->Context()->HSSetShader(m_buffer.Get(), nullptr, 0);
    }
    
}
#endif