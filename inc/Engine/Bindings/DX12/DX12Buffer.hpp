#pragma once

#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/DX12/UploadBuffer.hpp"

#include <d3d12.h>
#include <d3dx12.h>

#include <vector>

namespace orbit
{

    struct VertexBuffer
    {
        Ptr<ID3D12Resource> buffer = nullptr;
        D3D12_VERTEX_BUFFER_VIEW view = {};
    };

    struct IndexBuffer
    {
        Ptr<ID3D12Resource> buffer = nullptr;
        D3D12_INDEX_BUFFER_VIEW view = {};
    };

    struct ConstantBuffer
    {
        Ptr<ID3D12Resource> buffer = nullptr;
        D3D12_CONSTANT_BUFFER_VIEW_DESC view = {};
    };

    struct ShaderResourceBuffer
    {
        Ptr<ID3D12Resource> buffer;
        D3D12_SHADER_RESOURCE_VIEW_DESC view;  
    };

    struct UnorderedAccessBuffer
    {
        Ptr<ID3D12Resource> buffer = nullptr;
        D3D12_UNORDERED_ACCESS_VIEW_DESC view = {};
    };

    using VertexBufferView = D3D12_VERTEX_BUFFER_VIEW;
    using IndexBufferView = D3D12_INDEX_BUFFER_VIEW;
    using TextureBuffer = ShaderResourceBuffer;

    class VertexData;
    struct IndexData;

    struct Allocation;

    extern VertexBuffer CreateVertexBuffer(const VertexData& vData);
    extern IndexBuffer CreateIndexBuffer(const IndexData& iData);

    extern void UpdateVertexBuffer(VertexBuffer* buffer, const VertexData& vData);
    extern void UpdateIndexBuffer(IndexBuffer* buffer, const IndexData& iData);
    extern VertexBufferView MakeVertexBufferView(UploadBuffer::Allocation allocation, size_t sizeInBytes, size_t stride);

    template<class BufferPtrType>
    void UpdateBuffer(BufferPtrType buffer, size_t elementCount, size_t stride, void* memory)
    {
        //ORBIT_INFO("Lars, you still need to change this. Always flushing the COPY command queue is vastly inefficient!");
        auto cmdQ = Engine::Get()->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
        auto cmdList = cmdQ->GetCommandList();

        Ptr<ID3D12Resource> intermediate;
        UpdateBufferResource(
            cmdList,
            buffer->buffer,
            intermediate,
            elementCount,
            stride,
            memory
        );
        intermediate->SetName(L"Intermediate Buffer");

        cmdQ->ExecuteCommandList(cmdList);
        cmdQ->Flush();
    }

    template<class CopyType>
    extern ConstantBuffer CreateConstantBuffer(const CopyType& data, const wchar_t* name)
    {
        ConstantBuffer buffer;
        UpdateBuffer(&buffer, 1, sizeof(CopyType), (void*)&data);

        buffer.view.BufferLocation = buffer.buffer->GetGPUVirtualAddress();
        buffer.view.SizeInBytes = sizeof(CopyType);
        
        if (name)
            buffer.buffer->SetName(name);
        return buffer;
    }

}
