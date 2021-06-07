#include "Engine/Bindings/DX12/DX12Buffer.hpp"
#include "Engine/Rendering/Vertex.hpp"

#include "Engine/Engine.hpp"
#include "Engine/Misc/Logger.hpp"

namespace orbit
{

    VertexBuffer CreateVertexBuffer(const VertexData& vData)
    {
        VertexBuffer buffer;
        UpdateVertexBuffer(&buffer, vData);
        return buffer;
    }

    IndexBuffer CreateIndexBuffer(const IndexData& iData)
    {
        IndexBuffer buffer;
        UpdateIndexBuffer(&buffer, iData);
        return buffer;
    }

    VertexBufferView MakeVertexBufferView(UploadBuffer::Allocation allocation, size_t size, size_t stride)
    {
        return VertexBufferView{ allocation.GPU, static_cast<UINT>(size), static_cast<UINT>(stride) };
    }

    void UpdateVertexBuffer(VertexBuffer* buffer, const VertexData& vData)
    {
        UpdateBuffer(buffer, vData.GetVertexCount(), vData.GetStride(), vData.GetBuffer().memory);
        buffer->buffer->SetName(L"VertexBuffer");
        buffer->view.BufferLocation = buffer->buffer->GetGPUVirtualAddress();
        buffer->view.SizeInBytes = static_cast<UINT>(vData.GetBufferSize());
        buffer->view.StrideInBytes = static_cast<UINT>(vData.GetStride());
    }

    void UpdateIndexBuffer(IndexBuffer* buffer, const IndexData& iData)
    {
        UpdateBuffer(buffer, iData.indices.size(), sizeof(uint16_t), (void*)iData.indices.data());
        buffer->buffer->SetName(L"IndexBuffer");
        buffer->view.BufferLocation = buffer->buffer->GetGPUVirtualAddress();
        buffer->view.Format = iData.is16Bit ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        buffer->view.SizeInBytes = static_cast<UINT>(iData.indices.size() * sizeof(uint16_t));
    }


}