#pragma once
#include "BufferView.hpp"
#include "Helper.hpp"

namespace orbit
{

    template<class...Ts>
    class HeapBufferView : public BufferView<(sizeof(Ts) + ...)>
    {
    private:
        Byte* m_buffer = new Byte[buffer_size];
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
        HeapBufferView()
        {
            SetPointer(m_buffer);
        }
        virtual ~HeapBufferView()
        {
            if (m_buffer)
            {
                delete m_buffer;
                m_buffer = nullptr;
            }
        }
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