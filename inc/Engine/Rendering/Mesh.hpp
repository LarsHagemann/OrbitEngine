#pragma once
#include "Vertex.hpp"
#include "Engine/Bindings/Binding.hpp"
#include "Engine/Engine.hpp"

#include <vector>
#include <string>
#include <cassert>

namespace orbit
{

	// Default vertex layout
	struct DefaultVertex
	{
		Vector3f position;
		Vector3f normal;
		Vector3f tangent;
		Vector2f uv;
	};

	// @brief: a mesh might consist of several pieces with
	//	different materials. This stores information about
	//	a mesh subset
	struct SubMesh
	{
		size_t startVertex = 0u;
		size_t vertexCount = 0u;
		size_t startIndex = 0u;
		size_t indexCount = 0u;
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
		void AddSubMesh(SubMesh&& submesh) { _submeshes.emplace_back(std::move(submesh)); }

		const VertexData& GetVertexData() const { return _vertices; }
		const IndexData& GetIndexData() const { return _indices; }

		void RecalculateNormals(int vertexPositionOffset = -1, int vertexNormalOffset = -1);

		template<class Modifier>
		static std::shared_ptr<Mesh> CreatePlaneMesh(size_t subdivisions, std::string_view material, Modifier vertexModifier)
		{
			auto mesh = std::make_shared<Mesh>();
			assert(subdivisions <= 6 && "At most 6 subdivions are allowed for planes.");

			// Create vertices
			const auto verticesPerSide = static_cast<size_t>(std::pow(2, subdivisions) + 1);
			const auto numVertices = verticesPerSide * verticesPerSide;
			const auto oneOverVerticesPerSide = 1.f / (verticesPerSide - 1.f);
			const auto numQuads = static_cast<size_t>(std::pow(2, subdivisions * 2));
			const auto numIndices = 6 * numQuads;

			std::vector<DefaultVertex> vData;
			vData.reserve(verticesPerSide * verticesPerSide);

			auto vertices = Engine::Get()->CPUAllocate(sizeof(DefaultVertex) * numVertices);
			for (auto yidx = 0u; yidx < verticesPerSide; ++yidx)
			{
				const auto y = static_cast<float>(yidx) * oneOverVerticesPerSide;
				for (auto xidx = 0u; xidx < verticesPerSide; ++xidx)
				{
					const auto x = static_cast<float>(xidx) * oneOverVerticesPerSide;
					const auto idx = yidx * verticesPerSide + xidx;
					const auto memory_loc = idx * sizeof(DefaultVertex);

					DefaultVertex vertex;
					vertex.position = Vector3f{ x, y, 0.f };
					vertex.uv = Vector2f{ x, y };
					vertex.normal = Vector3f{ 0.f, 1.f, 0.f };
					vertexModifier(vertex.position);

					vData.emplace_back(std::move(vertex));
				}
			}

			memcpy_s(vertices.memory, sizeof(DefaultVertex) * verticesPerSide * verticesPerSide, vData.data(), sizeof(DefaultVertex) * verticesPerSide * verticesPerSide);

			mesh->_vertices.SetBuffer(vertices, sizeof(DefaultVertex), numVertices);
			mesh->_indices.indices.resize(numIndices); // We have 6 indices for every quad

			for (auto zidx = 0u; zidx < verticesPerSide - 1; ++zidx)
			{
				const auto rowIdx = zidx * verticesPerSide;
				for (auto xidx = 0u; xidx < verticesPerSide - 1; ++xidx)
				{
					const auto idx = rowIdx + xidx;
					const auto base_idx = (idx - zidx) * 6;
					mesh->_indices.indices[base_idx + 0] = static_cast<uint16_t>(idx);
					mesh->_indices.indices[base_idx + 1] = static_cast<uint16_t>(idx + 1);
					mesh->_indices.indices[base_idx + 2] = static_cast<uint16_t>(idx + verticesPerSide);
					mesh->_indices.indices[base_idx + 3] = static_cast<uint16_t>(idx + verticesPerSide);
					mesh->_indices.indices[base_idx + 4] = static_cast<uint16_t>(idx + 1);
					mesh->_indices.indices[base_idx + 5] = static_cast<uint16_t>(idx + verticesPerSide + 1);
				}
			}

			mesh->RecalculateNormals();

			SubMesh submesh;
			submesh.startVertex = 0u;
			submesh.vertexCount = static_cast<size_t>(numVertices);
			submesh.startIndex = 0u;
			submesh.indexCount = static_cast<size_t>(numIndices);
			submesh.pipelineState = "orbit/default";
			submesh.material = material;

			mesh->AddSubMesh(std::move(submesh));

			return mesh;
		}
		static std::shared_ptr<Mesh> CreatePlaneMesh(size_t subdivisions, std::string_view material);
	};

}