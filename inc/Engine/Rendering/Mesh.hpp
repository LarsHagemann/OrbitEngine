#pragma once
#include "Vertex.hpp"
#include "Engine/Bindings/Binding.hpp"

#include <vector>
#include <string>

namespace orbit
{

	// @brief: a mesh might consist of several pieces with
	//	different materials. This stores information about
	//	a mesh subset
	struct SubMesh
	{
		unsigned startVertex = 0u;
		unsigned vertexCount = 0u;
		unsigned startIndex = 0u;
		unsigned indexCount = 0u;
		std::string pipelineState;
		std::string material;
	};

	class Mesh
	{
	protected:
		friend class OrbLoader;
		// @member: generic vertex data container
		//	this container can store generic vertex data.
		//	the input layout (i.e. pipeline state) must be set appropriately
		VertexData _vertices;
		// @member: CPU index buffer
		//	by default this buffer stores 16-bit unsigned indices (2 indices per entry),
		//	but if the indices become to large (vertex count > 65565) we switch to
		//	32-bit unsigned indices
		IndexData _indices;
		// @member: GPU vertex buffer
		VertexBuffer _vertexBuffer;
		// @member: GPU index buffer
		IndexBuffer _indexBuffer;
		// @member: list of submeshes
		std::vector<SubMesh> _submeshes;
	protected:
		void ReloadBuffer();
	public:
		void LoadSubMesh(
			const VertexData& vertices,
			const IndexData& indices, 
			std::string_view materialId, 
			std::string_view pipelineStateId,
			size_t normalOffset = 0,
			size_t positionOffset = 0,
			bool recalculateNormals = false);

		VertexBufferView GetVertexBufferView() const;
		IndexBufferView GetIndexBufferView() const;
		size_t GetIndexCount() const;

		const std::vector<SubMesh>& GetSubmeshes() const { return _submeshes; }
	};

}