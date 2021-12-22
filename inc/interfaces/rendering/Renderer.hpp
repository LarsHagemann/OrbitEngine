#pragma once
#include "implementation/rendering/Submesh.hpp"

#include <unordered_map>

namespace orbit
{

    // The renderer is a base class telling Orbit how to render various objects
    // It must be implemented by the backend and registered accordingly
    class IRenderer
    {
    private:
        std::unordered_map<uint32_t, ResourceId> m_textures;
        ResourceId m_materialId = 0;
        ResourceId m_vsId = 0;
        ResourceId m_psId = 0;
        ResourceId m_gsId = 0;
        ResourceId m_dsId = 0;
        ResourceId m_hsId = 0;
    protected:
        virtual void BindTextureImpl(ResourceId id, uint32_t slot) const = 0;
        virtual void BindMaterialImpl(ResourceId id) const = 0;
        virtual void BindVertexShaderImpl(ResourceId id) const = 0;
        virtual void BindPixelShaderImpl(ResourceId id) const = 0;
        virtual void BindGeometryShaderImpl(ResourceId id) const = 0;
        virtual void BindDomainShaderImpl(ResourceId id) const = 0;
        virtual void BindHullShaderImpl(ResourceId id) const = 0;
    public:
        void BindTexture(ResourceId id, uint32_t slot);
        void BindMaterial(ResourceId id);
        void BindVertexShader(ResourceId id);
        void BindPixelShader(ResourceId id);
        void BindGeometryShader(ResourceId id);
        void BindDomainShader(ResourceId id);
        void BindHullShader(ResourceId id);
        virtual void Draw(const Submesh& submesh, uint32_t instanceCount) const = 0;
    };
    
}