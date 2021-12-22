#pragma once
#include <random>
#include <string>
#include "implementation/Common.hpp"

namespace orbit
{

    template<typename Source, typename Destination>
    using copy_const_t = std::conditional_t<
        std::is_const_v<Source>,
        std::add_const_t<Destination>,
        std::remove_const_t<Destination>
    >;

    namespace detail {
        
        template<Index index, class...Ts, Index...Indices>
        constexpr size_t CalculateOffsetImpl(std::integer_sequence<Index, Indices...>)
        {
            return ((sizeof(Ts) * (index > Indices ? 1 : 0)) + ...);
        }

    }

    template<size_t Size = 1024, class...Ts>
    std::string string_print(const char* format, Ts&&...ts)
    {
        char local[Size];
        auto written = sprintf_s(local, format, ts...);
        if constexpr (Size < 4096)
        {
            if (written == 0)
                return string_print<Size*2>(format, ts...);
        }
        return std::string(local, written);
    }

    static uint64_t random_orb_id()
    {
        static std::random_device device;
        static std::mt19937_64 engine(device());

        static std::uniform_int_distribution<uint64_t> dist;
        return dist(engine);
    }

    template<Index index, class...Ts>
    constexpr size_t CalculateOffset()
    {
        using IndexSeq = typename std::make_index_sequence<sizeof...(Ts)>;
        return detail::CalculateOffsetImpl<index, Ts...>(IndexSeq{});
    }

}