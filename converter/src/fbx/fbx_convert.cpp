#include "fbx/fbx_convert.hpp"
#include "fbx/fbx.hpp"
#include "orb/orb.hpp"
#include "orb/orb_type.hpp"

#include <execution>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cmath>

namespace fbx
{

	Quaterniond QuatFromEuler(Vector3d euler)
	{
		return 
			AngleAxisd(euler.x(), Vector3d::UnitZ()) *
			AngleAxisd(euler.y(), Vector3d::UnitX()) *
			AngleAxisd(euler.z(), Vector3d::UnitZ());
	}

	using namespace Eigen;

	MappingInformationType LoadMIT(const FBXNode* mitNode);
	ReferenceInformationType LoadRIT(const FBXNode* ritNode);
	void LoadFBXData(const FBXNode* root, FBXData* data);
	void LoadGeometries(const FBXNode* objectsNode, FBXData* data);
	void LoadMaterials(const FBXNode* objectsNode, FBXData* data);
	void LoadModels(const FBXNode* objectsNode, FBXData* data);
	void LoadConnections(const FBXNode* connectionsNode, FBXData* data);
	void LoadTextures(const FBXNode* objectsNode, FBXData* data);
	void LoadAttributes(const FBXNode* objectsNode, FBXData* data);
	void GetFBXGeometry(FBXGeometry* geometry, const FBXNode* geometryNode);
	void GetFBXMaterial(FBXMaterial* material, const FBXNode* materialNode);
	void FBXDataToOrb(const FBXData* data, orbit::Orb* orb);
	void FBXToIntermediate(const FBXData* data, FBXInterType* orb);
	void FBXGeometryAppendToMesh(const FBXGeometry* geometry, orbit::OrbMesh* mesh);
	void LoadPositions(const FBXGeometry* geometry, orbit::OrbMesh* mesh);
	void LoadNormals(const FBXGeometry* geometry, orbit::OrbMesh* mesh);
	void LoadTangents(const FBXGeometry* geometry, orbit::OrbMesh* mesh);
	void LoadUVs(const FBXGeometry* geometry, orbit::OrbMesh* mesh);
	void CleanupGeometry(orbit::OrbMesh* mesh);

	void ConnectModelModel(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXModel> m1, FBXInterType* intermediate);
	void ConnectModelAttribute(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXAttribute> attr, FBXInterType* intermediate);
	void ConnectModelGeometry(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXGeometry> geom, FBXInterType* intermediate);
	void ConnectModelMaterial(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXMaterial> mat, FBXInterType* intermediate);
	void ConnectMaterialTexture(std::shared_ptr<FBXMaterial> mat, std::shared_ptr<FBXTexture> tex, const std::string& channel);

	bool FBXToOrb(const FBXNode* rootNode, orbit::Orb* orb)
	{
		FBXData data;
		FBXInterModel intermediate;
		LoadFBXData(rootNode, &data);
		FBXDataToOrb(&data, orb);
		return true;
	}

	void FBXGeometryAppendToMesh(const FBXGeometry* geometry, orbit::OrbMesh* mesh)
	{
		orbit::OrbMesh tmpMesh;
		LoadPositions(geometry, &tmpMesh);
		LoadNormals(geometry, &tmpMesh);
		LoadTangents(geometry, &tmpMesh);
		LoadUVs(geometry, &tmpMesh);
		CleanupGeometry(&tmpMesh);

		orbit::SubMesh submesh;
		submesh.startIndex = static_cast<unsigned>(mesh->indices.size());
		submesh.indexCount = static_cast<unsigned>(tmpMesh.indices.size());
		submesh.startVertex = static_cast<unsigned>(mesh->vertices.size());
		submesh.vertexCount = static_cast<unsigned>(tmpMesh.vertices.size());
		mesh->indices.insert(
			mesh->indices.end(),
			tmpMesh.indices.begin(),
			tmpMesh.indices.end()
		);
		mesh->vertices.insert(
			mesh->vertices.end(),
			tmpMesh.vertices.begin(),
			tmpMesh.vertices.end()
		);
		mesh->submeshes.emplace_back(submesh);
	}

	void FBXToIntermediate(const FBXData* data, FBXInterType* intermediate)
	{
		for (const auto& connection : data->connections)
		{
			auto it0 = data->nodes.find(connection.id0);
			auto it1 = data->nodes.find(connection.id1);
			if (it0 == data->nodes.end() || it1 == data->nodes.end())
				continue;

			if (it0->second->type == FBXType::TYPE_MODEL
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelModel(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXModel>(it1->second), intermediate);
			else if (it0->second->type == FBXType::TYPE_ATTRIBUTE
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelAttribute(
					std::static_pointer_cast<FBXModel>(it1->second),
					std::static_pointer_cast<FBXAttribute>(it0->second), intermediate);
			else if (it1->second->type == FBXType::TYPE_ATTRIBUTE
				&& it0->second->type == FBXType::TYPE_MODEL)
				ConnectModelAttribute(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXAttribute>(it1->second), intermediate);
			else if (it0->second->type == FBXType::TYPE_GEOMETRY
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelGeometry(
					std::static_pointer_cast<FBXModel>(it1->second),
					std::static_pointer_cast<FBXGeometry>(it0->second), intermediate);
			else if (it1->second->type == FBXType::TYPE_GEOMETRY
				&& it0->second->type == FBXType::TYPE_MODEL)
				ConnectModelGeometry(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXGeometry>(it1->second), intermediate);
			else if (it0->second->type == FBXType::TYPE_MATERIAL
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelMaterial(
					std::static_pointer_cast<FBXModel>(it1->second),
					std::static_pointer_cast<FBXMaterial>(it0->second), intermediate);
			else if (it1->second->type == FBXType::TYPE_MATERIAL
				&& it0->second->type == FBXType::TYPE_MODEL)
				ConnectModelMaterial(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXMaterial>(it1->second), intermediate);
			else if (it0->second->type == FBXType::TYPE_MATERIAL
				&& it1->second->type == FBXType::TYPE_TEXTURE)
				ConnectMaterialTexture(
					std::static_pointer_cast<FBXMaterial>(it0->second),
					std::static_pointer_cast<FBXTexture>(it1->second), connection.propertyName);
			else if (it1->second->type == FBXType::TYPE_MATERIAL
				&& it0->second->type == FBXType::TYPE_TEXTURE)
				ConnectMaterialTexture(
					std::static_pointer_cast<FBXMaterial>(it1->second),
					std::static_pointer_cast<FBXTexture>(it0->second), connection.propertyName);
		}
	}

	void LoadPositions(const FBXGeometry* geometry, orbit::OrbMesh* mesh)
	{
		const auto& vertices = geometry->vertices;

		// A point is made of 3 doubles so the data has to contain
		// a multiple 3 doubles
		assert(vertices.size() % 3 == 0);
		std::vector<Vector3f> positions;
		positions.resize(vertices.size() / 3);

		for (auto i = 0u; i < vertices.size() / 3; ++i)
		{
			auto idx = i * 3;
			positions[i] = Vector3f{
				static_cast<float>(vertices[idx + 0ll]),
				static_cast<float>(vertices[idx + 1ll]),
				static_cast<float>(vertices[idx + 2ll])
			};
		}

		// vertex points are stored in positions
		//std::vector<int32_t> indices;
		auto indices = geometry->indices;
		if (indices.empty())
		{
			indices.resize(positions.size());
			std::iota(indices.begin(), indices.end(), 0);
		}
		// A negative index tells us that we reached the end 
		//	of a face
		unsigned faceVertexCount = 0;
		for (auto i = 0u; i < indices.size(); ++i)
		{
			++faceVertexCount;
			auto value = indices[i];
			if (value < 0) // We found a negative index
			{
				value = std::abs(value) - 1;
				if (faceVertexCount >= 4)
					throw "Orbit doesn't allow for quads or n-gons in meshes";

				faceVertexCount = 0;
				indices[i] = value;
			}
		}

		mesh->indices = { indices.begin(), indices.end() };
		mesh->vertices.resize(positions.size());
		for (auto i = 0u; i < positions.size(); ++i)
			mesh->vertices[i].position = positions[i];

		// transform from centimeters to meters
		std::for_each(
			mesh->vertices.begin(),
			mesh->vertices.end(),
			[](orbit::Vertex& vertex) { vertex.position *= 0.01f; }
		);
	}

	void LoadNormals(const FBXGeometry* geometry, orbit::OrbMesh* mesh)
	{
		auto indices = geometry->normals.normalIndices;
		auto normals = geometry->normals.normals;
		std::vector<Vector3f> bakedNormals(normals.size() / 3);
		for (auto i = 0u; i < normals.size() / 3; ++i)
		{
			auto idx = i * 3;
			bakedNormals[i] = Vector3f{
					static_cast<float>(normals[idx + 0ll]),
					static_cast<float>(normals[idx + 1ll]),
					static_cast<float>(normals[idx + 2ll])
			};
		}

		auto itd = geometry->normals.rit == ReferenceInformationType::REFERENCE_INDEX_TO_DIRECT;
		auto mx = itd ? indices.size() : bakedNormals.size();

		for (auto i = 0u; i < mx; ++i)
		{
			auto normal = bakedNormals[itd ? indices[i] : i];
			if (geometry->normals.mit == MappingInformationType::MAPPING_BY_VERTEX)
				mesh->vertices[i].normal += normal;
			else if (geometry->normals.mit == MappingInformationType::MAPPING_BY_POLYGON_VERTEX)
				mesh->vertices[mesh->indices[i]].normal += normal;
			else if (geometry->normals.mit == MappingInformationType::MAPPING_BY_POLYGON)
			{
				mesh->vertices[mesh->indices[i + 0ll]].normal += normal;
				mesh->vertices[mesh->indices[i + 1ll]].normal += normal;
				mesh->vertices[mesh->indices[i + 2ll]].normal += normal;
			}
		}
		for (auto& v : mesh->vertices)
			v.normal.normalize();
	}

	void LoadTangents(const FBXGeometry* geometry, orbit::OrbMesh* mesh)
	{
		auto indices = geometry->tangents.tangentIndices;
		auto tangents = geometry->tangents.tangents;
		std::vector<Vector3f> bakedTangents(tangents.size() / 3);
		for (auto i = 0u; i < tangents.size() / 3; ++i)
		{
			auto idx = i * 3;
			bakedTangents[i] = Vector3f{
					static_cast<float>(tangents[idx + 0ll]),
					static_cast<float>(tangents[idx + 1ll]),
					static_cast<float>(tangents[idx + 2ll])
			};
		}

		auto itd = geometry->tangents.rit == ReferenceInformationType::REFERENCE_INDEX_TO_DIRECT;
		auto mx = itd ? indices.size() : bakedTangents.size();

		for (auto i = 0u; i < mx; ++i)
		{
			auto tangent = bakedTangents[itd ? indices[i] : i];
			if (geometry->tangents.mit == MappingInformationType::MAPPING_BY_VERTEX)
				mesh->vertices[i].tangent += tangent;
			else if (geometry->tangents.mit == MappingInformationType::MAPPING_BY_POLYGON_VERTEX)
				mesh->vertices[mesh->indices[i]].tangent += tangent;
			else if (geometry->tangents.mit == MappingInformationType::MAPPING_BY_POLYGON)
			{
				mesh->vertices[mesh->indices[i + 0ll]].tangent += tangent;
				mesh->vertices[mesh->indices[i + 1ll]].tangent += tangent;
				mesh->vertices[mesh->indices[i + 2ll]].tangent += tangent;
			}
		}
		for (auto& v : mesh->vertices)
			v.tangent.normalize();
	}

	void LoadUVs(const FBXGeometry* geometry, orbit::OrbMesh* mesh)
	{
		auto tmpVertices = mesh->vertices;
		mesh->vertices.resize(mesh->indices.size());

		for (auto i = 0u; i < mesh->indices.size(); ++i)
			mesh->vertices[i] = tmpVertices[mesh->indices[i]];

		std::iota(mesh->indices.begin(), mesh->indices.end(), 0);

		// Create _positions.size() number of normals
		auto indices = geometry->uvs.uvIndices;
		std::vector<Vector2f> bakedUVs(geometry->uvs.uvs.size() / 2);
		for (auto i = 0u; i < geometry->uvs.uvs.size() / 2; ++i)
		{
			auto idx = i * 2;
			auto uv = Vector2f{
					static_cast<float>(geometry->uvs.uvs[idx + 0ll]),
					static_cast<float>(geometry->uvs.uvs[idx + 1ll])
			};
			bakedUVs[i] = uv;
		}

		auto itd = geometry->uvs.rit == ReferenceInformationType::REFERENCE_INDEX_TO_DIRECT;
		auto mx = itd ? indices.size() : bakedUVs.size();

		std::vector<int32_t> vv;
		auto ApplyUV = [&](unsigned i, Vector2f uv)
		{
			if (std::find(vv.begin(), vv.end(), i) == vv.end())
			{
				mesh->vertices[i].uv = uv;
				vv.emplace_back(i);
			}
			else
			{
				mesh->vertices.emplace_back(mesh->vertices[i]);
				mesh->vertices.back().uv = uv;
			}
		};

		for (auto i = 0u; i < mx; ++i)
		{
			auto uv = bakedUVs[itd ? indices[i] : i];
			if (geometry->tangents.mit == MappingInformationType::MAPPING_BY_VERTEX)
				ApplyUV(i, uv);
			else if (geometry->tangents.mit == MappingInformationType::MAPPING_BY_POLYGON_VERTEX)
				ApplyUV(mesh->indices[i], uv);
			else if (geometry->tangents.mit == MappingInformationType::MAPPING_BY_POLYGON)
			{
				ApplyUV(mesh->indices[i + 0ll], uv);
				ApplyUV(mesh->indices[i + 1ll], uv);
				ApplyUV(mesh->indices[i + 2ll], uv);
			}
		}

		mesh->indices.clear();
	}

	void CleanupGeometry(orbit::OrbMesh* mesh)
	{
		return;
		std::vector<orbit::Vertex> vertices;
		std::vector<uint16_t> indices;

		if (mesh->vertices.empty())
			return;

		bool cleanupNeccessary = false;
		vertices.emplace_back(mesh->vertices[0]);
		for (auto i = 1u; i < mesh->vertices.size(); ++i)
		{
			const auto& v0 = mesh->vertices[i];

			auto it = std::find_if(
				std::execution::par_unseq,
				vertices.begin(),
				vertices.end(),
				[v0](const orbit::Vertex& v) -> bool {
					return v0.position.isApprox(v.position) &&
						v0.normal.isApprox(v.normal) &&
						v0.tangent.isApprox(v.tangent) &&
						v0.uv.isApprox(v.uv);
				});

			if (it != vertices.end())
			{
				cleanupNeccessary = true;
				indices.emplace_back(static_cast<uint32_t>(std::distance(vertices.begin(), it)));
			}
			else
			{
				vertices.emplace_back(v0);
				indices.emplace_back(static_cast<uint32_t>(vertices.size() - 1));
			}
		}

		if (cleanupNeccessary)
		{
			mesh->vertices = vertices;
			mesh->indices = indices;
		}
	}

	void GetFBXGeometry(FBXGeometry* geometry, const FBXNode* geometryNode)
	{
		const FBXNode
			* verticesNode,
			* indicesNode,
			* normalParent,
			* tangentParent,
			* uvParent,
			* mitNode,
			* ritNode,
			* normalsNode,
			* normalIndexNode,
			* tangentNode,
			* tangentIndexNode,
			* uvNode,
			* uvIndexNode;

		if (geometryNode->properties.size() >= 2)
		{
			geometry->id = std::get<int64_t>(geometryNode->properties[0]);
			geometry->name = std::get<std::string>(geometryNode->properties[1]);
		}

		verticesNode = FindChild(geometryNode, "Vertices");
		indicesNode = FindChild(geometryNode, "PolygonVertexIndex");
		normalParent = FindChild(geometryNode, "LayerElementNormal");
		tangentParent = FindChild(geometryNode, "LayerElementTangent");
		uvParent = FindChild(geometryNode, "LayerElementUV");

		if (verticesNode && verticesNode->properties.size() == 1)
			geometry->vertices = std::get<std::vector<double>>(verticesNode->properties[0]);

		if (indicesNode && indicesNode->properties.size() == 1)
			geometry->indices = std::get<std::vector<int>>(indicesNode->properties[0]);

		if (normalParent)
		{
			normalsNode = FindChild(normalParent, "Normals");
			normalIndexNode = FindChild(normalParent, "NormalIndex");
			mitNode = FindChild(normalParent, "MappingInformationType");
			ritNode = FindChild(normalParent, "ReferenceInformationType");

			if (normalsNode && normalsNode->properties.size() == 1)
				geometry->normals.normals = std::get<std::vector<double>>(normalsNode->properties[0]);
			if (normalIndexNode && normalIndexNode->properties.size() == 1)
				geometry->normals.normalIndices = std::get<std::vector<int>>(normalIndexNode->properties[0]);

			geometry->normals.mit = LoadMIT(mitNode);
			geometry->normals.rit = LoadRIT(ritNode);
		}

		if (tangentParent)
		{
			tangentNode = FindChild(tangentParent, "Tangents");
			tangentIndexNode = FindChild(tangentParent, "TangentIndex");
			mitNode = FindChild(tangentParent, "MappingInformationType");
			ritNode = FindChild(tangentParent, "ReferenceInformationType");

			if (tangentNode && tangentNode->properties.size() == 1)
				geometry->tangents.tangents = std::get<std::vector<double>>(tangentNode->properties[0]);
			if (tangentIndexNode && tangentIndexNode->properties.size() == 1)
				geometry->tangents.tangentIndices = std::get<std::vector<int>>(tangentIndexNode->properties[0]);

			geometry->tangents.mit = LoadMIT(mitNode);
			geometry->tangents.rit = LoadRIT(ritNode);
		}

		if (uvParent)
		{
			uvNode = FindChild(uvParent, "UV");
			uvIndexNode = FindChild(uvParent, "UVIndex");
			mitNode = FindChild(uvParent, "MappingInformationType");
			ritNode = FindChild(uvParent, "ReferenceInformationType");

			if (uvNode && uvNode->properties.size() == 1)
				geometry->uvs.uvs = std::get<std::vector<double>>(uvNode->properties[0]);
			if (uvIndexNode && uvIndexNode->properties.size() == 1)
				geometry->uvs.uvIndices = std::get<std::vector<int>>(uvIndexNode->properties[0]);

			geometry->uvs.mit = LoadMIT(mitNode);
			geometry->uvs.rit = LoadRIT(ritNode);
		}
	}

	void GetFBXMaterial(FBXMaterial* material, const FBXNode* materialNode)
	{
		auto get_color = [](const FBXNode* node) -> Vector4f {
			return Vector4f{
				static_cast<float>(std::get<double>(node->properties[4])),
				static_cast<float>(std::get<double>(node->properties[5])),
				static_cast<float>(std::get<double>(node->properties[6])),
				1.f
			};
		};
		auto get_string = [](const FBXNode* node) {
			return std::get<std::string>(node->properties[0]);
		};

		material->id = std::get<int64_t>(materialNode->properties[0]);
		material->name = std::get<std::string>(materialNode->properties[1]);

		auto properties70 = FindChild(materialNode, "Properties70");
		if (!properties70) return;

		for (auto p : properties70->children)
		{
			auto channel = get_string(&p);
			if (channel == "DiffuseColor")
				material->diffuse = get_color(&p);
			if (channel == "DiffuseColor" || channel == "Maya|baseColor")
				material->diffuse.cwiseProduct(get_color(&p));
			if (channel == "DiffuseFactor")
				material->diffuse *= static_cast<float>(std::clamp(std::get<double>(p.properties[4]), 0., 1.));
			if (channel == "Maya|base")
				material->diffuse *= std::clamp(std::get<float>(p.properties[4]), 0.f, 1.f);
			if (channel == "Roughness")
				material->roughness = static_cast<float>(std::get<double>(p.properties[4]));
			if (channel == "Maya|specularRoughness")
				material->roughness = std::get<float>(p.properties[4]);
			if (channel == "Shininess")
				material->roughness = 1.f - static_cast<float>(std::get<double>(p.properties[4]));
		}
	}

	void FBXDataToOrb(const FBXData* data, orbit::Orb* orb)
	{
		FBXInterType intermediate;
		FBXToIntermediate(data, &intermediate);
		// intermediate to orb
		for (auto model : intermediate.models)
		{
			if (!model.second->geometries.empty())
			{
				orbit::OrbMesh mesh;
				auto modelName = model.second->model->modelName;
				mesh.name = modelName.substr(0, modelName.find('\0'));
				for (auto geometry : model.second->geometries)
					FBXGeometryAppendToMesh(geometry.get(), &mesh);

				orb->AddMesh(std::move(mesh));
			}
			for (auto material : model.second->materials)
			{
				orbit::OrbMaterial m0;
				m0.flags = 0u;
				m0.name = material->name.substr(0, material->name.find('\0'));
				m0.roughness = material->roughness;
				m0.diffuse = material->diffuse;
				
				for (auto texture : material->textures)
				{
					orbit::OrbTexture tex;
					tex.name = texture->name.substr(0, texture->name.find('\0'));
					tex.file = texture->filepath;
					if (texture->flags != TEXTURE_UNKNOWN)
					{
						if (texture->flags == TEXTURE_COLOR)
						{
							m0.flags |= (uint8_t)MaterialFlag::F_HAS_COLOR_MAP;
							m0.colorTextureId = tex.name;
						}
						else if (texture->flags == TEXTURE_ROUGHNESS)
						{
							m0.flags |= (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP;
							m0.roughnessTextureId = tex.name;
						}
						else if (texture->flags == TEXTURE_OCCLUSION)
						{
							m0.flags |= (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP;
							m0.occlusionTextureId = tex.name;
						}
						else if (texture->flags == TEXTURE_NORMAL)
						{
							m0.flags |= (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP;
							m0.normalTextureId = tex.name;
						}
						orb->AddTexture(std::move(tex));
					}
				}
				orb->AddMaterial(std::move(m0));
			}
		}
		for (auto light : intermediate.lights)
		{
			orbit::OrbLight l;
			l._color = light->color;
			l._direction = light->direction;
			l._falloffBegin = light->falloffBegin;
			l._falloffEnd = light->falloffEnd;
			l._position = light->position;
			l._spotAngle = light->spotAngle;
			l._type = (orbit::LightType)(uint8_t)light->ltype;
			orb->AddLight(std::move(l));
		}
	}

	MappingInformationType LoadMIT(const FBXNode* mitNode)
	{
		if (mitNode && mitNode->properties.size() == 1)
		{
			std::string mit = std::get<std::string>(mitNode->properties[0]);
			if (mit == "ByPolygonVertex")
				return MappingInformationType::MAPPING_BY_POLYGON_VERTEX;
			else if (mit == "ByVertex" || mit == "ByVertice")
				return MappingInformationType::MAPPING_BY_VERTEX;
			else if (mit == "ByPolygon")
				return MappingInformationType::MAPPING_BY_POLYGON;
		}

		return MappingInformationType::MAPPING_UNKNOWN;
	}

	ReferenceInformationType LoadRIT(const FBXNode* ritNode)
	{
		if (ritNode && ritNode->properties.size() == 1)
		{
			std::string rit = std::get<std::string>(ritNode->properties[0]);
			if (rit == "Direct")
				return ReferenceInformationType::REFERENCE_DIRECT;
			else if (rit == "IndexToDirect")
				return ReferenceInformationType::REFERENCE_INDEX_TO_DIRECT;
		}
		return ReferenceInformationType::REFERENCE_UNKNOWN;
	}

	void LoadFBXData(const FBXNode* root, FBXData* data)
	{
		auto objectsNode = FindChild(root, "Objects");
		auto connectionsNode = FindChild(root, "Connections");
		if (!objectsNode || !connectionsNode) return;

		LoadModels(objectsNode, data);
		LoadAttributes(objectsNode, data);
		LoadGeometries(objectsNode, data);
		LoadMaterials(objectsNode, data);
		LoadTextures(objectsNode, data);

		LoadConnections(connectionsNode, data);
	}

	void LoadGeometries(const FBXNode* objectsNode, FBXData* data)
	{
		const FBXNode* geometryNode;
		auto idx = 0u;
		while ((geometryNode = fbx::FindChild(objectsNode, "Geometry", idx++)) != nullptr)
		{
			FBXGeometry fbx_geom;
			fbx_geom.type = FBXType::TYPE_GEOMETRY;
			GetFBXGeometry(&fbx_geom, geometryNode);

			data->nodes.emplace(fbx_geom.id, std::make_shared<FBXGeometry>(std::move(fbx_geom)));
		
		}
	}

	void LoadMaterials(const FBXNode* objectsNode, FBXData* data)
	{
		const FBXNode* materialNode;
		auto idx = 0u;
		while ((materialNode = fbx::FindChild(objectsNode, "Material", idx++)) != nullptr)
		{
			FBXMaterial fbx_mat;
			fbx_mat.type = FBXType::TYPE_MATERIAL;
			GetFBXMaterial(&fbx_mat, materialNode);

			data->nodes.emplace(fbx_mat.id, std::make_shared<FBXMaterial>(std::move(fbx_mat)));
		}
	}

	void LoadModels(const FBXNode* objectsNode, FBXData* data)
	{
		auto Vec3FromNode = [](const FBXNode* node) -> Vector3d
		{
			auto x = std::get<double>(node->properties[4]);
			auto y = std::get<double>(node->properties[4]);
			auto z = std::get<double>(node->properties[4]);

			return Vector3d{ x, y, z };
		};

		const FBXNode* modelNode;
		auto idx = 0u;
		while ((modelNode = FindChild(objectsNode, "Model", idx++)) != nullptr)
		{
			FBXModel model;
			model.type = FBXType::TYPE_MODEL;
			model.id = std::get<int64_t>(modelNode->properties[0]);
			model.modelName = std::get<std::string>(modelNode->properties[1]);
			model.modelType = std::get<std::string>(modelNode->properties[2]);
			if (auto p70 = FindChild(modelNode, "Properties70"); p70 != nullptr)
			{
				const FBXNode* pNode;
				auto idx0 = 0u;
				while ((pNode = FindChild(p70, "P", idx0++)) != nullptr)
				{
					auto channel = std::get<std::string>(pNode->properties[0]);
					if (channel == "Lcl Translation")
						model.transform.position = Vec3FromNode(pNode);
					else if (channel == "Lcl Rotation")
						model.transform.rotation = QuatFromEuler(Vec3FromNode(pNode));
					else if (channel == "Lcl Scaling")
						model.transform.scaling = Vec3FromNode(pNode);
				}
			}
			data->nodes.emplace(model.id, std::make_shared<FBXModel>(std::move(model)));
		}
	}

	void LoadConnections(const FBXNode* connectionsNode, FBXData* data)
	{
		const FBXNode* connectionNode;
		auto idx = 0u;
		while ((connectionNode = FindChild(connectionsNode, "C", idx++)) != nullptr)
		{
			FBXConnection connection;
			auto type = std::get<std::string>(connectionNode->properties[0]);
			connection.id0 = std::get<int64_t>(connectionNode->properties[1]);
			connection.id1 = std::get<int64_t>(connectionNode->properties[2]);
			if (connectionNode->properties.size() > 3)
				connection.propertyName = std::get<std::string>(connectionNode->properties[3]);

			if (type == "OO")
				connection.type = FBXConnectionType::CT_OBJECT_OBJECT;
			else if (type == "OP")
				connection.type = FBXConnectionType::CT_OBJECT_PROPERTY;
			else if (type == "PO")
				connection.type = FBXConnectionType::CT_PROPERTY_OBJECT;
			else if (type == "PP")
				connection.type = FBXConnectionType::CT_PROPERTY_PROPERTY;

			data->connections.emplace_back(std::move(connection));
		}
	}

	void LoadTextures(const FBXNode* objectsNode, FBXData* data)
	{
		const FBXNode* textureNode;
		auto idx = 0u;
		while ((textureNode = FindChild(objectsNode, "Texture", idx++)) != nullptr)
		{
			FBXTexture texture;
			texture.type = FBXType::TYPE_TEXTURE;
			texture.id = std::get<int64_t>(textureNode->properties[0]);
			texture.name = std::get<std::string>(textureNode->properties[1]);

			if (auto filenameNode = FindChild(textureNode, "FileName"); filenameNode != nullptr)
				texture.filepath = std::get<std::string>(filenameNode->properties[0]);
			
			data->nodes.emplace(texture.id, std::make_shared<FBXTexture>(std::move(texture)));
		}
	}

	void LoadAttributes(const FBXNode* objectsNode, FBXData* data)
	{
		const FBXNode* attributeNode;
		auto idx = 0u;
		while ((attributeNode = FindChild(objectsNode, "NodeAttribute", idx++)) != nullptr)
		{
			FBXAttribute attribute;
			attribute.type = FBXType::TYPE_ATTRIBUTE;
			attribute.id = std::get<int64_t>(attributeNode->properties[0]);
			attribute.nodeType = std::get<std::string>(attributeNode->properties[1]);
			attribute.nodeName = std::get<std::string>(attributeNode->properties[2]);
			const FBXNode* p70;
			if ((p70 = FindChild(attributeNode, "Properties70")); p70 != nullptr)
				attribute.attributes = p70;

			data->nodes.emplace(attribute.id, std::make_shared<FBXAttribute>(std::move(attribute)));
		}
	}

	std::unordered_map<int64_t, std::shared_ptr<FBXInterModel>>::iterator TryInsert(std::shared_ptr<FBXModel> model, FBXInterType* intermediate)
	{
		auto it = intermediate->models.find(model->id);
		if (it != intermediate->models.end())
			return it;

		FBXInterModel inter;
		inter.model = model;
		inter.id = model->id;
		inter.type = FBXType::TYPE_INTERMODEL;
		intermediate->models.emplace(inter.id, std::make_shared<FBXInterModel>(inter));
		return intermediate->models.find(inter.id);
	}

	void ConnectModelModel(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXModel> m1, FBXInterType* intermediate)
	{
		auto im = TryInsert(m0, intermediate)->second;
		im->children.emplace_back(m1);

		m1->transform.parent = &m0->transform;
	}

	void ConnectModelAttribute(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXAttribute> attr, FBXInterType* intermediate)
	{
		auto im = TryInsert(m0, intermediate)->second;
		im->attributes.emplace_back(attr);

		auto attribute_name = [](const FBXNode& node)
		{
			return std::get<std::string>(node.properties[0]);
		};
		auto get_color = [](const FBXNode& node) -> Vector4f
		{
			Vector4f color;
			color.x() = static_cast<float>(std::get<double>(node.properties[4]));
			color.y() = static_cast<float>(std::get<double>(node.properties[5]));
			color.z() = static_cast<float>(std::get<double>(node.properties[6]));
			color.w() = 1.f;
			return color;
		};

		if (attr->nodeName == "Light")
		{
			FBXLight light;
			for (const auto& attribute : attr->attributes->children)
			{
				auto name = attribute_name(attribute);
				if (name == "LightType")
				{
					auto light_type = std::get<int32_t>(attribute.properties[4]);
					if (light_type == 0)
						light.ltype = FBXLightType::PointLight;
					else if (light_type == 2)
						light.ltype = FBXLightType::SpotLight;
					else if (light_type == 3)
						light.ltype = FBXLightType::DirectionalLight;
				}
				else if (name == "Color")
					light.color = get_color(attribute);
				else if (name == "DecayStart")
				{
					light.falloffBegin = static_cast<float>(std::get<double>(attribute.properties[4]));
					light.falloffEnd = light.falloffBegin * 1.2f;
				}
				else if (name == "OuterAngle")
					light.spotAngle = static_cast<float>(std::get<double>(attribute.properties[4]));
			}
			auto p = m0->transform.GetCombinedPosition();
			auto d = m0->transform.GetCombinedRotation()._transformVector(Vector3d::UnitX());
			light.position = Vector4f{
				static_cast<float>(p.x()),
				static_cast<float>(p.y()),
				static_cast<float>(p.z()),
				1.f
			};
			light.direction = Vector4f{
				static_cast<float>(d.x()),
				static_cast<float>(d.y()),
				static_cast<float>(d.z()),
				0.f
			};
			intermediate->lights.emplace_back(std::make_shared<FBXLight>(light));
		}
	}

	void ConnectModelGeometry(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXGeometry> geom, FBXInterType* intermediate)
	{
		auto im = TryInsert(m0, intermediate)->second;
		im->geometries.emplace_back(geom);
	}

	void ConnectModelMaterial(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXMaterial> mat, FBXInterType* intermediate)
	{
		auto im = TryInsert(m0, intermediate)->second;
		im->materials.emplace_back(mat);
	}

	void ConnectMaterialTexture(std::shared_ptr<FBXMaterial> mat, std::shared_ptr<FBXTexture> tex, const std::string& channel)
	{
		tex->flags = TEXTURE_UNKNOWN;
		if (channel == "Maya|specularRoughness")
		{
			//mat->flags |= MaterialFlag::F_HAS_ROUGHNESS_MAP;
			tex->flags = TEXTURE_ROUGHNESS;
		}
		else if (channel == "Maya|normalCamera")
		{
			//mat->flags |= MaterialFlag::F_HAS_NORMAL_MAP;
			tex->flags = TEXTURE_NORMAL;
		}
		else if (channel == "Maya|baseColor" || channel == "DiffuseColor")
		{
			//mat->flags |= MaterialFlag::F_HAS_COLOR_MAP;
			tex->flags = TEXTURE_COLOR;
		}
		mat->textures.emplace_back(tex);
	}

}