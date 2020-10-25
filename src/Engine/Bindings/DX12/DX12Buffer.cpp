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
        buffer.buffer->SetName(L"VertexBuffer");
        return buffer;
    }

    IndexBuffer CreateIndexBuffer(const IndexData& iData)
    {
        IndexBuffer buffer;
        UpdateIndexBuffer(&buffer, iData);
        buffer.buffer->SetName(L"IndexBuffer");
        return buffer;
    }

    void UpdateVertexBuffer(VertexBuffer* buffer, const VertexData& vData)
    {
        ORBIT_INFO("Lars, you still need to change this. Always flushing the COPY command queue is vastly inefficient!");
        auto cmdQ = Engine::Get()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
        auto cmdList = cmdQ->GetCommandList();

        Ptr<ID3D12Resource> intermediate;
        UpdateBufferResource(
            cmdList,
            buffer->buffer,
            intermediate,
            vData.GetVertexCount(),
            vData.GetStride(),
            vData.GetBuffer().memory
        );
        intermediate->SetName(L"Intermediate Buffer");

        cmdQ->ExecuteCommandList(cmdList);
        buffer->view.BufferLocation = buffer->buffer->GetGPUVirtualAddress();
        buffer->view.SizeInBytes = static_cast<UINT>(vData.GetBufferSize());
        buffer->view.StrideInBytes = static_cast<UINT>(vData.GetStride());
        cmdQ->Flush();
    }

    void UpdateIndexBuffer(IndexBuffer* buffer, const IndexData& iData)
    {
        ORBIT_INFO("Lars, you still need to change this. Always flushing the COPY command queue is vastly inefficient!");
        auto cmdQ = Engine::Get()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
        auto cmdList = cmdQ->GetCommandList();

        Ptr<ID3D12Resource> intermediate;
        UpdateBufferResource(
            cmdList,
            buffer->buffer,
            intermediate,
            iData.indices.size(),
            sizeof(uint16_t),
            (void*)iData.indices.data()
        );

        cmdQ->ExecuteCommandList(cmdList);
        buffer->view.BufferLocation = buffer->buffer->GetGPUVirtualAddress();
        buffer->view.Format = iData.is16Bit ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        buffer->view.SizeInBytes = static_cast<UINT>(iData.indices.size() * sizeof(uint16_t));
        cmdQ->Flush();
    }

    VertexBufferView MakeVertexBufferView(UploadBuffer::Allocation allocation, size_t size, size_t stride)
    {
        return VertexBufferView{ allocation.GPU, static_cast<UINT>(size), static_cast<UINT>(stride) };
    }


}