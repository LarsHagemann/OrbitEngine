#pragma once
#include "interfaces/misc/Bindable.hpp"

#include <cstdint>
#include <vector>

namespace orbit
{

    template<typename BufferType, typename Vertex>
    class IVertexBufferBase : public IBindable<uint32_t, uint32_t, uint32_t>
    {
    public:
        using vertex_type = Vertex;
    protected:
        std::vector<Vertex> m_vertices;
        BufferType m_buffer;
    public:
        uint32_t NumVertices() const { return m_vertices.size(); }
        int32_t IndexAt(uint32_t index) const { return m_vertices.at(index); }
        uint32_t GetBufferSize() const { return NumVertices() * sizeof(Vertex); }
        BufferType GetBuffer() const { return m_buffer; }
        const std::vector<Vertex>& GetVertices() const { return m_vertices; }
        void ResizeBuffer(uint32_t numElements) { m_vertices.resize(numElements); }
        void SetVertex(uint32_t index, const Vertex& vertex) { m_vertices[index] = vertex; }
        void SetVertices(std::vector<Vertex>&& vertices) { m_vertices = std::move(vertices); }
        void SetVertices(const std::vector<Vertex>& vertices) { m_vertices = vertices; }

        virtual void UpdateBuffer() = 0;
    };

}