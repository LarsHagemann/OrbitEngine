#pragma once
#include <bitset>

namespace orbit
{

    template<size_t NumBits>
    class BitField : public std::bitset<NumBits>
    {
    public:
        template<typename...T>
        BitField(T...bits)
        {
            (std::bitset<NumBits>::set(static_cast<size_t>(bits)), ...);
        }
        template<typename T>
        bool test(T pos) const
        {
            return std::bitset<NumBits>::test(static_cast<size_t>(pos));
        }
        template<typename...T>
        bool test(T...poss) const
        {
            return (std::bitset<NumBits>::test(static_cast<size_t>(poss)) && ...);
        }
    };

}