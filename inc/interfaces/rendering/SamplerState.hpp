#pragma once
#include "interfaces/misc/UnLoadable.hpp"
#include "interfaces/misc/Bindable.hpp"

namespace orbit
{

    class ISamplerState : public UnLoadable, public IBindable<uint32_t>
    {
    private:
    public:
        
    };

}
