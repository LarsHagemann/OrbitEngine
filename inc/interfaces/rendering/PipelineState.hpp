#pragma once
#include "implementation/Common.hpp"
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

namespace orbit
{

    class IPipelineState : public IBindable<>, public UnLoadable
    {
    protected:
        ResourceId m_vertexShaderId;
        ResourceId m_pixelShaderId;
        ResourceId m_geometryShaderId;
        ResourceId m_domainShaderId;
        ResourceId m_hullShaderId;
        ResourceId m_inputLayoutId;
        ResourceId m_blendStateId;
        ResourceId m_rasterizerStateId;
        ResourceId m_rootSignatureId;
    public:
        virtual void Bind() const override;
        bool LoadImpl(std::ifstream* stream) override;
        void UnloadImpl() override;
    };

}