#include "mesh.hpp"

namespace orbit
{

	void Mesh::ReloadBuffer()
	{
	}

	void Mesh::LoadSubMesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::string_view materialId, bool recalculateNormals)
	{
		const auto preVertexCount = _vertices.size();
		const auto preIndexCount = _indices.size();

		_vertices.insert(
			_vertices.end(),
			vertices.begin(),
			vertices.end()
		);
		_indices.insert(
			_indices.end(),
			indices.begin(),
			indices.end()
		);
		std::transform(
			_indices.begin() + preIndexCount,	// first index to be transformed
			_indices.end(),						// first index not to be transformed
			_indices.begin() + preIndexCount,	// first index of the destination
			[preVertexCount](uint32_t& idx) { return idx + preVertexCount; }	// modifier function
		);

		if (recalculateNormals)
		{
			for (auto i = preVertexCount; i < preVertexCount + _vertices.size(); ++i)
				_vertices[i].normal = Vector3f::Zero();

			for (auto i = preIndexCount; i < preIndexCount + _indices.size(); i += 3)
			{
				auto v0 = _vertices[_indices[i + 0]].position;
				auto v1 = _vertices[_indices[i + 1]].position;
				auto v2 = _vertices[_indices[i + 2]].position;

				auto n = (v1 - v0).cross(v2 - v1);

				_vertices[_indices[i + 0]].normal += n;
				_vertices[_indices[i + 1]].normal += n;
				_vertices[_indices[i + 2]].normal += n;
			}

			for (auto i = preVertexCount; i < preVertexCount + _vertices.size(); ++i)
				_vertices[i].normal.normalize();
		}

		SubMesh m;
		m.startVertex = preVertexCount;
		m.vertexCount = _vertices.size();
		m.startIndex = preIndexCount;
		m.indexCount = _indices.size();
		m.material = materialId;
		
		_submeshes.emplace_back(std::move(m));
		ReloadBuffer();
	}

}