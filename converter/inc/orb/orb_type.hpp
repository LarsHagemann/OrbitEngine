#pragma once
#include <vector>
#include <string>
#include <filesystem>
#include "Eigen/Dense"
#include "../textures.hpp"

namespace orbit
{

	namespace fs = std::filesystem;
	using namespace Eigen;
	using namespace tex;

	// @brief: defines a single vertex
	struct Vertex
	{
		// @member: position of the vertex in 3d space
		Vector3f position = Vector3f::Zero();
		// @member: normal of the vertex
		Vector3f normal = Vector3f::Zero();
		// @member: tangent of the vertex
		Vector3f tangent = Vector3f::Zero();
		// @member: uv coordinates of the vertex
		Vector2f uv = Vector2f::Zero();
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
		std::string pipelineState;
		std::string material;
	};

	struct OrbMesh
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;
		std::vector<SubMesh> submeshes;
	};

	struct OrbMaterial
	{
		std::string name;
		// @member: the diffuse color (how the object appears)
		Vector4f diffuse = Vector4f::Zero();
		// @member: roughness of the mesh
		float roughness = 0.f;
		// @member: flags for the material
		uint32_t flags = 0;
		// @member: id of the color texture (might be empty)
		std::string colorTextureId;
		// @member: id of the normal texture (might be empty)
		std::string normalTextureId;
		// @member: id of the occlusion texture (might be empty)
		std::string occlusionTextureId;
		// @member: id of the roughness texture (might be empty)
		std::string roughnessTextureId;
	};

	// @brief: the type of light.
	enum class LightType
	{
		DirectionalLight,
		PointLight,
		SpotLight
	};

	// @brief: holds information about a light in the scene
	//	therefore this struct holds information that all of the light types need
	struct OrbLight
	{
		// @member: the color of the light. All lights share this property
		Vector4f _color = Vector4f::Zero();
		// @member: the position of the light. Only point- and spotlights
		Vector4f _position = Vector4f::Zero();
		// @member: the direction of the light rays. Only directional- and spotlights
		Vector4f _direction = Vector4f::Zero();
		// @member: the angle of a spotlight cone. Only spotlights
		float _spotAngle = 0.f;
		// @member: the distance at which to begin dimming the light. Only point- and spotlights
		float _falloffBegin = 0.f;
		// @member: the maximum distance of a light source. Only point- and spotlights
		float _falloffEnd = 0.f;
		// @member: the type of the light
		LightType _type;
	};

	struct OrbTexture
	{
		std::string name;
		fs::path file;
	};

	struct OrbVersion
	{
		union {
			struct {
				uint16_t major;
				uint16_t revision;
			} _v0;
			uint32_t _v1;
		} high; // high order 4 bytes
		char build_type;
		char build;

		uint16_t unused__ = 0U;
	};

	static std::ostream& operator<<(std::ostream& out, const OrbVersion& version)
	{
		return out
			<< 'v' << version.build_type
			<< version.high._v0.major << '.'
			<< version.high._v0.revision << '-'
			<< version.build;
	}

#ifdef _DEBUG
	static const OrbVersion tool_version = OrbVersion{ 3, 3, 'D', 'b' };
#else
	static const OrbVersion tool_version = OrbVersion{ 3, 3, 'R', 'b' };
#endif

}