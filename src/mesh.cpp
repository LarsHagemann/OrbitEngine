#include "mesh.hpp"
#include "engine.hpp"

namespace orbit
{

	void Mesh::ReloadBuffer()
	{
		auto cmdQ = _engine->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
		auto cmdList = cmdQ->GetCommandList();

		Ptr<ID3D12Resource> intermediateVertexBuffer;
		UpdateBufferResource(
			_engine->GetDevice(),
			cmdList.Get(),
			_vertexBuffer.GetAddressOf(),
			intermediateVertexBuffer.GetAddressOf(),
			_vertices.size(),
			sizeof(Vertex),
			_vertices.data()
		);

		_vertexBufferView.BufferLocation = _vertexBuffer->GetGPUVirtualAddress();
		_vertexBufferView.SizeInBytes = _vertices.size() * sizeof(Vertex);
		_vertexBufferView.StrideInBytes = sizeof(Vertex);

		Ptr<ID3D12Resource> intermediateIndexBuffer;
		UpdateBufferResource(
			_engine->GetDevice(),
			cmdList.Get(),
			_indexBuffer.GetAddressOf(),
			intermediateIndexBuffer.GetAddressOf(),
			_indices.size(),
			sizeof(uint32_t),
			_indices.data()
		);

		_indexBufferView.BufferLocation = _indexBuffer->GetGPUVirtualAddress();
		_indexBufferView.SizeInBytes = _indices.size() * sizeof(uint32_t);
		_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

		auto fence = cmdQ->ExecuteCommandList(cmdList);
		cmdQ->WaitForFenceValue(fence);
	}

	void Mesh::LoadSubMesh(
		std::vector<Vertex>&& vertices, 
		std::vector<uint32_t>&& indices, 
		std::string_view materialId, 
		std::string_view pipelineStateId,
		bool recalculateNormals)
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
		m.pipelineState = pipelineStateId;
		
		_submeshes.emplace_back(std::move(m));
		ReloadBuffer();
	}

}