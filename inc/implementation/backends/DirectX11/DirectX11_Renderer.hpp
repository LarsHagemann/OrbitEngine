#pragma once
#ifdef ORBIT_DIRECTX_11
#include "interfaces/rendering/Renderer.hpp"

namespace orbit
{

    class DirectX11Renderer : public IRenderer
    {
    protected:
        void BindTextureImpl(ResourceId id, uint32_t slot) const override;
        void BindMaterialImpl(ResourceId id) const override;
        void BindVertexShaderImpl(ResourceId id) const override;
        void BindPixelShaderImpl(ResourceId id) const override;
        void BindGeometryShaderImpl(ResourceId id) const override;
        void BindDomainShaderImpl(ResourceId id) const override;
        void BindHullShaderImpl(ResourceId id) const override;
    public:
        void Draw(const Submesh& submesh, uint32_t instanceCount) const override;
    };

}
#endif