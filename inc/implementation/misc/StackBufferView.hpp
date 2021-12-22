#pragma once
#include "BufferView.hpp"
#include "Helper.hpp"

namespace orbit
{

    template<class...Ts>
    class StackBufferView : public BufferView<(sizeof(Ts) + ...)>
    {
    private:
        Byte m_buffer[buffer_size];
    private:
        template<Index index, typename This>
        auto GetPointerToObject(This) const
        {
            constexpr auto offset = CalculateOffset<index, Ts...>();
            static_assert(offset < buffer_size);
            using IndexType = copy_const_t<
                This,
                std::remove_reference_t<
                    decltype(std::get<index>(std::tuple<Ts...>{}))
                >
            >;
            return reinterpret_cast<IndexType*>(m_buffer + offset);
        }
    public:
        StackBufferView() :
            BufferView<(sizeof(Ts) + ...)>(m_buffer)
        {}
        template<Index index>
        auto GetPointerToObject()
        {
            return GetPointerToObject<index>(this);
        }
        template<Index index>
        auto GetPointerToObject() const
        {
            return GetPointerToObject<index>(this);
        }
    };

}