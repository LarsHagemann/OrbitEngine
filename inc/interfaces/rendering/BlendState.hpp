#pragma once
#include "interfaces/misc/UnLoadable.hpp"
#include "interfaces/misc/Bindable.hpp"

namespace orbit
{

    class IBlendState : public UnLoadable, public IBindable<>
    {
    protected:
        float m_blendFactor[4];
        int32_t m_sampleMask;
    public:
        virtual bool LoadImpl(std::ifstream* stream) override;
    };

}
