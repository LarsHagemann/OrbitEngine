#pragma once
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

namespace orbit
{

    class IRasterizerState : public IBindable<>, public UnLoadable
    {
    private:
    public:
    };

}