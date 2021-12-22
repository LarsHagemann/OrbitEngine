#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_PixelShader.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/engine/Engine.hpp"

#include <d3dcompiler.h>

namespace orbit
{

    HRESULT DirectX11PixelShader::CreateShader(const char* binary, size_t binarySize)
    {
        return ENGINE->Device()->CreatePixelShader(binary, binarySize, nullptr, m_buffer.ReleaseAndGetAddressOf());
    }

    void DirectX11PixelShader::Bind() const
    {
        ENGINE->Context()->PSSetShader(m_buffer.Get(), nullptr, 0);
    }
    
}
#endif