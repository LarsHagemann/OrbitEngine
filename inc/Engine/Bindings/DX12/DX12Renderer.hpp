#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/DX12/DX12Buffer.hpp"
#include "Engine/Rendering/RenderState.hpp"
#include <d3d12.h>

#include <string>

namespace orbit
{

    class Renderer
    {
    protected:
        Ptr<ID3D12GraphicsCommandList> _commandList;
        RenderState _currentRenderState;
        std::string _currentMaterial;
        std::string _currentPipelineState;
        std::string _currentRootSignature;
    public:
        Renderer(Ptr<ID3D12GraphicsCommandList> cmdList);
        void PrepareRenderState(RenderState state, bool enforce = false);
        void BindVertexBuffers(size_t startSlot, size_t numBuffers, VertexBufferView* views);
        void BindIndexBuffer(const IndexBufferView& view);
        void BindConstantBuffer(unsigned rootParameterIndex, const UploadBuffer::Allocation& allocation);
        void BindMaterial(const std::string& id);
        void BindPipelineState(const std::string& id);
        void BindRootSignature(const std::string& id);
        void DrawInstanced(size_t vertexCount, size_t instanceCount, size_t startVertex, size_t startInstance);
        void DrawIndexedInstanced(size_t indexCount, size_t instanceCount, size_t startIndex, size_t startVertex, size_t startInstance);
    };

}
