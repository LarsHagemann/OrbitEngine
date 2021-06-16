#include "Engine/Rendering/Mesh.hpp"
#include "Engine/Engine.hpp"

namespace orbit
{

    void Mesh::ReloadBuffer()
    {
        UpdateVertexBuffer(&_vertexBuffer, _vertices);
        if (_indices.size() > 0)
            UpdateIndexBuffer(&_indexBuffer, _indices);
    }

    void Mesh::LoadSubMesh(
		const VertexData& vertices, 
		const IndexData& indices, 
		std::string_view materialId, 
		std::string_view pipelineStateId,
        size_t normalOffset,
        size_t positionOffset,
		bool recalculateNormals)
	{
        assert(vertices.GetStride() == _vertices.GetStride() && "You may not change the Vertex Layout mid mesh!");

		const auto preVertexCount = _vertices.GetVertexCount();
		const auto preIndexCount = _indices.size();
        

        const auto combinedSize = _vertices.GetBufferSize() + vertices.GetBufferSize();
        const auto totalVertices = _vertices.GetVertexCount() + vertices.GetVertexCount();

        auto allocation = Engine::Get()->CPUAllocate(
            combinedSize
        );

        memcpy_s(allocation.memory, combinedSize, _vertices.GetBuffer().memory, _vertices.GetBufferSize());
        memcpy_s(
            (uint8_t*)allocation.memory + _vertices.GetBufferSize(), vertices.GetBufferSize(), 
            vertices.GetBuffer().memory, vertices.GetBufferSize());

        _vertices.GetBuffer().Free();
        _vertices.SetBuffer(allocation, vertices.GetStride(), totalVertices);
        vertices.GetBuffer().Free();

        CombineIndexData(_indices, indices, preVertexCount);

		if (recalculateNormals)
		{
            auto get_normal = [&](size_t i) -> Vector3f&
            {
                static auto stride = _vertices.GetStride();
                static auto vertex = (uint8_t*)_vertices.GetBuffer().memory + i * stride;

                return *((Vector3f*)vertex + normalOffset);
            };
            auto get_position = [&](size_t i) -> Vector3f&
            {
                static auto stride = _vertices.GetStride();
                static auto vertex = (uint8_t*)_vertices.GetBuffer().memory + i * stride;

                return *((Vector3f*)vertex + positionOffset);
            };

			for (auto i = preVertexCount; i < preVertexCount + _vertices.GetVertexCount(); ++i)
				get_normal(i) = Vector3f::Zero();

			for (auto i = preIndexCount; i < preIndexCount + _indices.size(); i += 3)
			{
				auto v0 = get_position(_indices.at(i + 0));
				auto v1 = get_position(_indices.at(i + 1));
				auto v2 = get_position(_indices.at(i + 2));

				auto n = (v1 - v0).cross(v2 - v1);

                get_normal(_indices.at(i + 0)) += n;
                get_normal(_indices.at(i + 1)) += n;
                get_normal(_indices.at(i + 2)) += n;
			}

			for (auto i = preVertexCount; i < preVertexCount + _vertices.GetVertexCount(); ++i)
                get_normal(i).normalize();
		}

		SubMesh m;
		m.startVertex = static_cast<unsigned>(preVertexCount);
		m.vertexCount = static_cast<unsigned>(vertices.GetVertexCount());
		m.startIndex = static_cast<unsigned>(preIndexCount);
		m.indexCount = static_cast<unsigned>(indices.size());
		m.material = materialId;
		m.pipelineState = pipelineStateId;
		
		_submeshes.emplace_back(std::move(m));
		ReloadBuffer();
	}

    void Mesh::RecalculateNormals(int vertexPositionOffset,int vertexNormalOffset)
    {
        if (vertexNormalOffset < 0)
            vertexNormalOffset = 12; // Default normal offset is 12 bytes
        if (vertexPositionOffset < 0)
            vertexPositionOffset = 0; // Default position offset is 0 bytes

        auto normalAt = [&](size_t index) {
            return (Vector3f*)((char*)_vertices.GetBuffer().memory + index * _vertices.GetStride() + vertexNormalOffset);
        };
        auto positionAt = [&](size_t index) {
            return *(Vector3f*)((char*)_vertices.GetBuffer().memory + index * _vertices.GetStride() + vertexPositionOffset);
        };

        for (auto i = 0u; i < _vertices.GetVertexCount(); ++i)
            *normalAt(i) = Vector3f::Zero();

        for (auto i = 0u; i < _indices.size() - 2; ++i)
        {
            auto 
                v1 = positionAt(_indices.indices.at(i + 0)),
                v2 = positionAt(_indices.indices.at(i + 1)),
                v3 = positionAt(_indices.indices.at(i + 2));

            auto normal = (v2 - v1).cross(v3 - v1);

            *normalAt(_indices.indices.at(i + 0)) += normal;
            *normalAt(_indices.indices.at(i + 1)) += normal;
            *normalAt(_indices.indices.at(i + 2)) += normal;
        }

        for (auto i = 0u; i < _vertices.GetVertexCount(); ++i)
            normalAt(i)->normalize();

        ReloadBuffer();
    }

    VertexBufferView Mesh::GetVertexBufferView() const
    {
        return _vertexBuffer.view;
    }

    IndexBufferView Mesh::GetIndexBufferView() const
    {
        return _indexBuffer.view;
    }

    size_t Mesh::GetIndexCount() const
    {
        return _indices.size();
    }

    std::shared_ptr<Mesh> Mesh::CreatePlaneMesh(size_t subdivisions, std::string_view material)
    {
        return CreatePlaneMesh(subdivisions, material, [](Vector3f& position) {});
    }

}
