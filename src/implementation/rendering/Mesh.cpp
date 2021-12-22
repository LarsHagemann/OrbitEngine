#include "implementation/rendering/Mesh.hpp"
#include "implementation/engine/Engine.hpp"

#include <fstream>
#include <limits>

namespace orbit
{

    Mesh::Mesh()
    {
    }

    void Mesh::Bind() const
    {
        if (m_indexBuffer)
            m_indexBuffer->Bind(0);
        if (m_vertexBuffer)
            m_vertexBuffer->Bind(0, sizeof(Vertex), 0);
    }

    void Mesh::Draw(uint32_t instanceCount, uint32_t submesh) const
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

    bool Mesh::LoadImpl(std::ifstream* stream)
    {
        m_indexBuffer = std::make_unique<IndexBuffer>();
        m_vertexBuffer = std::make_unique<VertexBuffer<Vertex>>();

        uint32_t nameLen = 0;
        Submesh mesh;
        std::vector<int32_t> indices;
        std::vector<Vertex> vertices;
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
        stream->read((char*)vertices.data(), sizeof(Vertex) * mesh.vertexCount);

        m_vertexBuffer->SetVertices(std::move(vertices));
        m_indexBuffer->SetIndices(std::move(indices));
        m_vertexBuffer->UpdateBuffer();
        m_indexBuffer->UpdateBuffer();

        m_submeshes.emplace_back(mesh);
        return true;
    }
    
    void Mesh::UnloadImpl()
    {
        m_indexBuffer = nullptr;
        m_vertexBuffer = nullptr;

        m_submeshes.clear();
    }

}