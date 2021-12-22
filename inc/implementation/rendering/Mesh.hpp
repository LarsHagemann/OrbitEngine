#pragma once
#include "implementation/backends/Platform.hpp"
#include "implementation/rendering/Submesh.hpp"
#include "implementation/rendering/Vertex.hpp"
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

#include <memory>

namespace orbit
{

    class Mesh : public IBindable<>, public UnLoadable
    {
    private:
        UPtr<IndexBuffer> m_indexBuffer;
        UPtr<VertexBuffer<Vertex>> m_vertexBuffer;
        std::vector<Submesh> m_submeshes;
        ResourceId m_id;
    public:
        Mesh();
        virtual void Bind() const override;
        // @param submesh: The submesh to be drawn.
        //  Use 0xFFFFFFFF to draw all submeshes
        void Draw(uint32_t instanceCount, uint32_t submesh = 0xFFFFFFFF) const;
        bool LoadImpl(std::ifstream* stream) override;
        void UnloadImpl() override;
        const IndexBuffer* GetIndexBuffer() const { return m_indexBuffer.get(); }
        const VertexBuffer<Vertex>* GetVertexBuffer() const { return m_vertexBuffer.get(); }
    };

}