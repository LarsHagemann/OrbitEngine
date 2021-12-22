#pragma once

namespace orbit
{

    template<typename...Params>
    class IBindable
    {
    public:
        virtual void Bind(Params...params) const = 0;
    };

}