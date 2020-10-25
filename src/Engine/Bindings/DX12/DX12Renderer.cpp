#include "Engine/Bindings/DX12/DX12Renderer.hpp"
#include "Engine/Engine.hpp"

namespace orbit
{

    //static bool operator!=(const D3D12_VIEWPORT& a, const D3D12_VIEWPORT& b)
    //{
    //    return 
    //        (a.Height != b.Height) ||
    //        (a.Width != b.Width) ||
    //        (a.MinDepth != b.MinDepth) ||
    //        (a.MaxDepth != b.MaxDepth) ||
    //        (a.TopLeftX != b.TopLeftX) ||
    //        (a.TopLeftY != b.TopLeftY);
    //}

    static bool operator==(const D3D12_RECT& a, const D3D12_RECT& b) noexcept
    {
        return
            (a.bottom == b.bottom) &&
            (a.top == b.top) &&
            (a.left == b.left) &&
            (a.right == b.right);
    }

    static bool operator!=(const D3D12_RECT& a, const D3D12_RECT& b) noexcept
    {
        return !(a == b);
    }

    Renderer::Renderer(Ptr<ID3D12GraphicsCommandList> cmdList) : 
        _commandList(cmdList)
    {
        PrepareRenderState(_currentRenderState, true);
    }

    void Renderer::PrepareRenderState(RenderState state, bool enforce)
    {
        if (state.viewport != _currentRenderState.viewport || enforce)
            if (state.viewport == RenderState::sEmptyViewport)
                _commandList->RSSetViewports(0, nullptr);
            else _commandList->RSSetViewports(1, &state.viewport);

        if (state.scissorRect != _currentRenderState.scissorRect || enforce)
            if (state.scissorRect == RenderState::sEmptyRect)
                _commandList->RSSetScissorRects(0, nullptr);
            else _commandList->RSSetScissorRects(1, &state.scissorRect);

        _currentRenderState = state;
    }

    void Renderer::BindVertexBuffers(size_t startSlot, size_t numBuffers, VertexBufferView* views)
    {
        std::vector<D3D12_VERTEX_BUFFER_VIEW> buffers(numBuffers);
        for (auto i = 0u; i < numBuffers; ++i)
        {
            buffers[i].BufferLocation = views[i].BufferLocation;
            buffers[i].SizeInBytes = static_cast<UINT>(views[i].SizeInBytes);
            buffers[i].StrideInBytes = static_cast<UINT>(views[i].StrideInBytes);
        }
        _commandList->IASetVertexBuffers(static_cast<UINT>(startSlot), static_cast<UINT>(numBuffers), buffers.data());
    }

    void Renderer::BindIndexBuffer(const IndexBufferView& view)
    {
        _commandList->IASetIndexBuffer(&view);
    }

    void Renderer::BindConstantBuffer(unsigned rootParameterIndex, const UploadBuffer::Allocation& allocation)
    {
        _commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, allocation.GPU);
    }

    void Renderer::BindMaterial(const std::string& id)
    {
        if (_currentMaterial != id)
        {


            _currentMaterial = id;
        }
    }

    void Renderer::BindPipelineState(const std::string& id)
    {
        if (_currentPipelineState != id)
        {
            auto pso = Engine::Get()->GetPipelineState(id);
            if (!pso) return;

            _commandList->SetPipelineState(pso.Get());
            _currentPipelineState = id;
        }
    }

    void Renderer::BindRootSignature(const std::string& id)
    {
        if (_currentRootSignature != id)
        {
            auto rs = Engine::Get()->GetRootSignature(id);
            if (!rs) return;

            _commandList->SetGraphicsRootSignature(rs.Get());
            _currentRootSignature = id;
        }
    }

    void Renderer::DrawInstanced(size_t vertexCount, size_t instanceCount, size_t startVertex, size_t startInstance)
    {
        _commandList->DrawInstanced(
            static_cast<UINT>(vertexCount), 
            static_cast<UINT>(instanceCount), 
            static_cast<UINT>(startVertex), 
            static_cast<UINT>(startInstance)
        );
    }

    void Renderer::DrawIndexedInstanced(size_t indexCount, size_t instanceCount, size_t startIndex, size_t startVertex, size_t startInstance)
    {
        _commandList->DrawIndexedInstanced(
            static_cast<UINT>(indexCount), 
            static_cast<UINT>(instanceCount), 
            static_cast<UINT>(startIndex), 
            static_cast<UINT>(startVertex), 
            static_cast<UINT>(startInstance)
        );
    }

}