#include "interfaces/rendering/Renderer.hpp"

namespace orbit
{

    void IRenderer::BindTexture(ResourceId id, uint32_t slot)
    {
        auto it = m_textures.find(slot);
        if (it != m_textures.end() && it->second == id)
            return;
        
        m_textures[slot] = id;
        BindTextureImpl(id, slot);
    }

    void IRenderer::BindMaterial(ResourceId id)
    {
        if (m_materialId == id)
            return;
        
        m_materialId = id;
        BindMaterialImpl(id);
    }

    void IRenderer::BindVertexShader(ResourceId id)
    {
        if (m_vsId == id)
            return;
        
        m_vsId = id;
        BindVertexShaderImpl(id);
    }

    void IRenderer::BindPixelShader(ResourceId id)
    {
        if (m_psId == id)
            return;
        
        m_psId = id;
        BindPixelShaderImpl(id);
    }

    void IRenderer::BindGeometryShader(ResourceId id)
    {
        if (m_gsId == id)
            return;
        
        m_gsId = id;
        BindGeometryShaderImpl(id);
    }

    void IRenderer::BindDomainShader(ResourceId id)
    {
        if (m_dsId == id)
            return;
        
        m_dsId = id;
        BindDomainShaderImpl(id);
    }

    void IRenderer::BindHullShader(ResourceId id)
    {
        if (m_hsId == id)
            return;
        
        m_hsId = id;
        BindHullShaderImpl(id);
    }

}