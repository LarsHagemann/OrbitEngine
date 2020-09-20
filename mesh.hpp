#pragma once
#include "helper.hpp"
#include "material.hpp"

namespace orbit
{

	// @brief: defines a single vertex
	struct Vertex
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
		unsigned startVertex = 0u;
		unsigned vertexCount = 0u;
		unsigned startIndex = 0u;
		unsigned indexCount = 0u;
		std::string material;
	};

	// @brief: the mesh describes geometry that might consist 
	//	of several submeshes
	class Mesh
	{
	protected:
		// @member: list of all the vertices used by this mesh
		std::vector<Vertex> _vertices;
		// @member: list of all the indices used by this mesh
		std::vector<uint32_t> _indices;
		// @member: list of all the submeshes
		std::vector<SubMesh> _submeshes;
	protected:
		// @method: reloads the internal dx12 buffer
		void ReloadBuffer();
	public:
		// @brief: loads a submesh
		// @param vertices: vertices of the submesh
		// @param indices: indices of the submesh (might be empty)
		// @param recalculateNormals: will recalculate the vertex normals if set to true
		void LoadSubMesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices, std::string_view materialId = "", bool recalculateNormals = false);
	};

}
