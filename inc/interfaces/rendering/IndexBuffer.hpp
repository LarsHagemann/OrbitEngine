#pragma once
#include "interfaces/misc/Bindable.hpp"

#include <cstdint>
#include <vector>

namespace orbit
{

    template<typename BufferType>
    class IIndexBufferBase : public IBindable<uint32_t>
    {
    protected:
        std::vector<int32_t> m_indices;
        BufferType m_buffer;
    public:
        uint32_t NumIndices() const { return m_indices.size(); }
        int32_t IndexAt(uint32_t index) const { return m_indices.at(index); }
        uint32_t GetBufferSize() const { return NumIndices() * sizeof(uint32_t); }
        virtual BufferType GetBuffer() const { return m_buffer; }
        const std::vector<int32_t>& GetIndices() const { return m_indices; }
        void ResizeBuffer(uint32_t numElements) { m_indices.resize(numElements); }
        void SetIndex(uint32_t index, uint32_t value) { m_indices[index] = value; }
        void SetIndices(std::vector<int32_t>&& indices) { m_indices = std::move(indices); }
        void SetIndices(const std::vector<int32_t>& indices) { m_indices = indices; }

        virtual void UpdateBuffer() = 0;
    };

}