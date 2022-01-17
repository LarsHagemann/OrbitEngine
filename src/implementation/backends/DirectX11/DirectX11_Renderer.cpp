#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_Renderer.hpp"
#include "implementation/engine/Engine.hpp"
#include "implementation/backends/impl/PipelineStateImpl.hpp"

#include "interfaces/rendering/Material.hpp"

namespace orbit
{

    void DirectX11Renderer::Draw(const Submesh& submesh, uint32_t instanceCount) const
    {
        if (submesh.pipelineStateId != m_currentPipelineState)
        {
            m_currentPipelineState = submesh.pipelineStateId;
            ENGINE->RMLoadResource<PipelineState>(submesh.pipelineStateId)->Bind();
        }
        if (submesh.materialId != m_currentMaterial)
        {
            m_currentMaterial = submesh.materialId;
            ENGINE->RMLoadResource<MaterialBase>(submesh.materialId)->Bind(1);
        }

        if (submesh.indexCount > 0)
            ENGINE->Context()->DrawIndexedInstanced(submesh.indexCount, instanceCount, submesh.startIndex, submesh.startVertex, 0);
        else
            ENGINE->Context()->DrawInstanced(submesh.vertexCount, instanceCount, submesh.startVertex, 0);
    }

    void DirectX11Renderer::BindTextureImpl(ResourceId id, uint32_t slot) const
    {
        ENGINE->RMLoadResource<Texture>(id)->Bind(slot);
    }

    void DirectX11Renderer::BindMaterialImpl(ResourceId id) const
    {
        ENGINE->RMLoadResource<MaterialBase>(id)->Bind(1);
    }

    void DirectX11Renderer::BindVertexShaderImpl(ResourceId id) const
    {
        ENGINE->RMLoadResource<VertexShader>(id)->Bind();
    }

    void DirectX11Renderer::BindPixelShaderImpl(ResourceId id) const
    {
        ENGINE->RMLoadResource<PixelShader>(id)->Bind();
    }

    void DirectX11Renderer::BindGeometryShaderImpl(ResourceId id) const
    {
        ENGINE->RMLoadResource<GeometryShader>(id)->Bind();
    }

    void DirectX11Renderer::BindDomainShaderImpl(ResourceId id) const
    {
        ENGINE->RMLoadResource<DomainShader>(id)->Bind();
    }

    void DirectX11Renderer::BindHullShaderImpl(ResourceId id) const
    {
        ENGINE->RMLoadResource<HullShader>(id)->Bind();
    }

}
#endif