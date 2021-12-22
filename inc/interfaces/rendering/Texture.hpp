#pragma once
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

namespace orbit
{

    template<class TextureDataContainer>
    class ITextureBase : public IBindable<uint32_t>, public UnLoadable
    {
    protected:
        TextureDataContainer m_textureContainer;
    };

}