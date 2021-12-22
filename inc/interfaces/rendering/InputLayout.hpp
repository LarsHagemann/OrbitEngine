#pragma once
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

namespace orbit
{

    template<typename InputLayoutType>
    class IInputLayout : public IBindable<>, public UnLoadable
    {
    protected:
        ResourceId m_id;
        InputLayoutType m_inputLayout;
    };

}