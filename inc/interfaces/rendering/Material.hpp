#pragma once
#include <memory>

#include "implementation/Common.hpp"
#include "implementation/misc/Color.hpp"
#include "implementation/misc/Helper.hpp"
#include "implementation/backends/impl/ConstantBufferImpl.hpp"
#include "implementation/rendering/MaterialFlags.hpp"
#include "interfaces/misc/UnLoadable.hpp"
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/Loggable.hpp"

namespace orbit
{

    class MaterialBase : public UnLoadable, public IBindable<uint32_t>
    {
    protected:
        using BufferType = ConstantBuffer<
            Albedo,
            Specular,
            Roughness,
            MaterialFlags
        >;
        std::unique_ptr<BufferType> m_buffer;
        ResourceId      m_albedoTexture = 0;
        ResourceId      m_normalTexture = 0;
        ResourceId      m_roughnessTexture = 0;
        ResourceId      m_occlusionTexture = 0;
    public:
        void UnloadImpl() override;
        bool LoadImpl(std::ifstream* stream) override;
        // @method: Binds this material
        virtual void Bind(uint32_t slot) const override;
    };

}