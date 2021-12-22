#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_IndexBuffer.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    void DirectX11IndexBuffer::UpdateBuffer()
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = GetBufferSize();
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA indexData;
        ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
        indexData.pSysMem = m_indices.data();

        if (FAILED(ENGINE->Device()->CreateBuffer(&desc, &indexData, m_buffer.ReleaseAndGetAddressOf())))
            ORBIT_ERROR("Failed to create buffer");
    }

    void DirectX11IndexBuffer::Bind(uint32_t offset) const
    {
        ENGINE->Context()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);
    }

}

#endif