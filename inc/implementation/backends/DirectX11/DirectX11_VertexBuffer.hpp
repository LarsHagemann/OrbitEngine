#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"

#include "interfaces/rendering/VertexBuffer.hpp"

namespace orbit
{

    template<typename Vertex>
    class DirectX11VertexBuffer : public IVertexBufferBase<ComPtr<ID3D11Buffer>, Vertex>
    {
    public:
        void UpdateBuffer() override
        {
            D3D11_BUFFER_DESC desc;
            ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.ByteWidth = GetBufferSize();
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            desc.StructureByteStride = sizeof(Vertex);

            D3D11_SUBRESOURCE_DATA vertexData;
            ZeroMemory(&vertexData, sizeof(D3D11_SUBRESOURCE_DATA));
            vertexData.pSysMem = m_vertices.data();

            if (FAILED(Engine::Get()->Device()->CreateBuffer(&desc, &vertexData, m_buffer.ReleaseAndGetAddressOf())))
                ORBIT_ERROR("Failed to create buffer");
        }
        void Bind(uint32_t slot, uint32_t stride, uint32_t offset) const override
        {
            Engine::Get()->Context()->IASetVertexBuffers(slot, 1, m_buffer.GetAddressOf(), &stride, &offset);
        }
    };

}
#endif