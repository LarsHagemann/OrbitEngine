#pragma once
#include "Helper.hpp"

namespace orbit
{

    template<size_t Size>
    class BufferView
    {
    private:
        void* m_bufferPointer;
    protected:
        void SetPointer(void* ptr) { m_bufferPointer = ptr; }
    public:
        BufferView() :
            m_bufferPointer()
        {}

        constexpr static auto buffer_size = Size;
        virtual ~BufferView() {}

        virtual const Byte* GetBufferPointer() const { return (Byte*)m_bufferPointer; }
        virtual Byte* GetBufferPointer() { return (Byte*)m_bufferPointer; }
    };

}