#pragma once
#include "Eigen/Dense"
#include "../textures.hpp"

#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

namespace fbx
{

	using namespace Eigen;
	using namespace tex;
	namespace fs = std::filesystem;

	struct FBXNode;

	struct FBXTransform
	{
		Vector3d position = Vector3d::Zero();
		Quaterniond rotation = Quaterniond::Identity();
		Vector3d scaling = Vector3d::Zero();
		FBXTransform* parent = nullptr;
		Quaterniond GetCombinedRotation()
		{
			if (parent != nullptr)
				return parent->GetCombinedRotation() * rotation;
			return rotation;
		}
		Vector3d GetCombinedPosition()
		{
			if (parent != nullptr)
				return parent->GetCombinedPosition() + position;
			return position;
		}
	};

	enum class FBXType
	{
		TYPE_MODEL,
		TYPE_GEOMETRY,
		TYPE_TEXTURE,
		TYPE_MATERIAL,
		TYPE_ATTRIBUTE,
		TYPE_INTERMODEL
	};

	struct FBXBase
	{
		int64_t id = 0;
		FBXType type;
	};

	enum class MappingInformationType
	{
		MAPPING_BY_POLYGON_VERTEX,
		MAPPING_BY_VERTEX,
		MAPPING_BY_POLYGON,

		MAPPING_UNKNOWN
	};

	enum class ReferenceInformationType
	{
		REFERENCE_DIRECT,
		REFERENCE_INDEX_TO_DIRECT,

		REFERENCE_UNKNOWN
	};

	struct NormalInfo
	{
		std::vector<double> normals;
		std::vector<int> normalIndices;
		MappingInformationType mit;
		ReferenceInformationType rit;
	};

	struct TangentInfo
	{
		std::vector<double> tangents;
		std::vector<int> tangentIndices;
		MappingInformationType mit;
		ReferenceInformationType rit;
	};

	struct UVInfo
	{
		std::vector<double> uvs;
		std::vector<int> uvIndices;
		MappingInformationType mit;
		ReferenceInformationType rit;
	};

	struct FBXGeometry : public FBXBase
	{
		std::string name;
		std::vector<double> vertices;
		std::vector<int> indices;
		NormalInfo normals;
		TangentInfo tangents;
		UVInfo uvs;
	};

	struct FBXModel : public FBXBase
	{
		FBXTransform transform;
		std::string modelType;
		std::string modelName;
	};

	struct FBXTexture : public FBXBase
	{
		fs::path filepath;
		std::string name;
		uint8_t flags = 0;
	};

	struct FBXMaterial : public FBXBase
	{
		std::string name;
		Vector4f diffuse = Vector4f::Zero();
		float roughness = 0.f;
		std::vector<std::shared_ptr<FBXTexture>> textures;
	};

	// @brief: the type of light.
	enum class FBXLightType
	{
		DirectionalLight,
		PointLight,
		SpotLight
	};

	struct FBXLight : public FBXBase
	{
		// @member: the color of the light. All lights share this property
		Vector4f color = Vector4f::Zero();
		// @member: the position of the light. Only point- and spotlights
		Vector4f position = Vector4f::Zero();
		// @member: the direction of the light rays. Only directional- and spotlights
		Vector4f direction = Vector4f::Zero();
		// @member: the angle of a spotlight cone. Only spotlights
		float spotAngle = 0.f;
		// @member: the distance at which to begin dimming the light. Only point- and spotlights
		float falloffBegin = 0.f;
		// @member: the maximum distance of a light source. Only point- and spotlights
		float falloffEnd = 0.f;
		// @member: the type of the light
		FBXLightType ltype;
	};

	struct FBXAttribute : public FBXBase
	{
		std::string nodeType;
		std::string nodeName;
		const FBXNode* attributes;
	};

	enum class FBXConnectionType
	{
		CT_OBJECT_OBJECT,
		CT_OBJECT_PROPERTY,
		CT_PROPERTY_OBJECT,
		CT_PROPERTY_PROPERTY
	};

	struct FBXConnection
	{
		FBXConnectionType type;
		std::string propertyName;
		int64_t id0 = 0;
		int64_t id1 = 0;
	};

	struct FBXData
	{
		std::unordered_map<int64_t, std::shared_ptr<FBXBase>> nodes;
		std::vector<FBXConnection> connections;
	};

	struct FBXInterModel : public FBXBase
	{
		std::shared_ptr<FBXModel> model;
		std::vector<std::shared_ptr<FBXGeometry>> geometries;
		std::vector<std::shared_ptr<FBXAttribute>> attributes;
		std::vector<std::shared_ptr<FBXMaterial>> materials;
		std::vector<std::shared_ptr<FBXModel>> children;
	};

	struct FBXInterType
	{
		std::unordered_map<int64_t, std::shared_ptr<FBXInterModel>> models;
		std::vector<std::shared_ptr<FBXLight>> lights;
	};

}
