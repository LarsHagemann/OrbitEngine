#pragma once
#include "implementation/backends/Platform.hpp"
#include "implementation/rendering/Submesh.hpp"
#include "interfaces/misc/Bindable.hpp"
#include "interfaces/misc/UnLoadable.hpp"

#include <memory>

namespace orbit
{

    template<class VertexType>
    class Mesh : public IBindable<>, public UnLoadable
    {
    private:
        UPtr<IndexBuffer> m_indexBuffer;
        UPtr<VertexBuffer<VertexType>> m_vertexBuffer;
        std::vector<Submesh> m_submeshes;
        ResourceId m_id;
    public:
        virtual void Bind() const override
        {
            if (m_indexBuffer)
                m_indexBuffer->Bind(0);
            if (m_vertexBuffer)
                m_vertexBuffer->Bind(0, sizeof(VertexType), 0);
        }

        // @param submesh: The submesh to be drawn.
        //  Use 0xFFFFFFFF to draw all submeshes
        void Draw(uint32_t instanceCount, uint32_t submesh = 0xFFFFFFFF) const
        {
            if (submesh == std::numeric_limits<uint32_t>::max())
            {
                for (const auto& submesh : m_submeshes)
                    ENGINE->Renderer()->Draw(submesh, instanceCount);
            }
            else
            {
                ENGINE->Renderer()->Draw(m_submeshes.at(submesh), instanceCount);
            }
        }

        bool LoadImpl(std::ifstream* stream) override
        {
            m_indexBuffer = std::make_unique<IndexBuffer>();
            m_vertexBuffer = std::make_unique<VertexBuffer<VertexType>>();

            uint32_t nameLen = 0;
            Submesh mesh;
            std::vector<int32_t> indices;
            std::vector<VertexType> vertices;
            stream->read((char*)&mesh.materialId, sizeof(ResourceId));
            mesh.materialId += GetId();
            if (mesh.materialId == GetId())
                mesh.materialId = ENGINE->RMGetIdFromName("materials/default");

            stream->read((char*)&mesh.indexCount, sizeof(uint64_t));
            stream->read((char*)&mesh.vertexCount, sizeof(uint64_t));
            indices.resize(mesh.indexCount);
            vertices.resize(mesh.vertexCount);

            mesh.startIndex = 0u;
            mesh.startVertex = 0u;
            mesh.pipelineStateId = ENGINE->RMGetIdFromName("pipeline_states/default");

            stream->read((char*)indices.data(), sizeof(int32_t) * mesh.indexCount);
            stream->read((char*)vertices.data(), sizeof(VertexType) * mesh.vertexCount);

            m_vertexBuffer->SetVertices(std::move(vertices));
            m_indexBuffer->SetIndices(std::move(indices));
            m_vertexBuffer->UpdateBuffer();
            m_indexBuffer->UpdateBuffer();

            m_submeshes.emplace_back(mesh);
            return true;
        }

        void UnloadImpl() override
        {
            m_indexBuffer = nullptr;
            m_vertexBuffer = nullptr;

            m_submeshes.clear();
        }

        const IndexBuffer* GetIndexBuffer() const { return m_indexBuffer.get(); }
        const VertexBuffer<VertexType>* GetVertexBuffer() const { return m_vertexBuffer.get(); }
        void SetVertexBuffer(VertexBuffer<VertexType>& buffer) { m_vertexBuffer = std::make_unique<VertexBuffer<VertexType>>(buffer); }
        void SetIndexBuffer(IndexBuffer& buffer) { m_indexBuffer = std::make_unique<IndexBuffer>(buffer); }

        void AddSubmesh(const Submesh& submesh)
        {
            m_submeshes.emplace_back(submesh);
        }
    };

}