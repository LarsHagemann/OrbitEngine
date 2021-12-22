#include "fbx/FbxReader.hpp"
#include "fbx/FbxTree.hpp"
#include "implementation/misc/Logger.hpp"

#include <iostream>
#include <numeric>
#include <algorithm>
#include <execution>

namespace orbtool
{

    static float Sigmoid(float value)
	{
		return 1.f / (1 + std::exp(-value));
	}

    bool FbxReader::ReadFile(const fs::path& filepath, OrbIntermediate* orb)
    {
        if (!OpenFile(filepath))
            return false;

        FbxTree tree(&m_file);
        FBXData data;
        LoadFBXData(tree.GetRootNode(), &data);
        FBXToIntermediate(&data);

        for (auto model : m_fbx.models)
		{
			if (!model.second->geometries.empty())
			{
                OrbMesh mesh;
				auto modelName = model.second->model->modelName;
				for (auto geometry : model.second->geometries)
					FBXGeometryAppendToMesh(geometry.get(), &mesh);

				if (!model.second->materials.empty())
				{
					auto name = model.second->materials[0]->name;
					name = name.substr(0, name.find('\0'));
					mesh.material = name;
				}
                orb->AppendObject(modelName.substr(0, modelName.find('\0')), std::move(mesh));
			}
			for (auto material : model.second->materials)
			{
				OrbMaterial m0;
				auto name = material->name.substr(0, material->name.find('\0'));
				m0.roughness = material->roughness;
				m0.diffuse = material->diffuse;
				
				for (auto texture : material->textures)
				{
					OrbTexture tex;
					auto tName = texture->name.substr(0, texture->name.find('\0'));
					tex.texturePath = texture->filepath;
					if (texture->flags != TextureType::TEXTURE_OTHER)
					{
						if (texture->flags == TextureType::TEXTURE_COLOR)
						{
                            m0.diffuseTextureId = tName;
						}
						else if (texture->flags == TextureType::TEXTURE_ROUGHNESS)
						{
							m0.roughnessMapId = tName;
						}
						else if (texture->flags == TextureType::TEXTURE_OCCLUSION)
						{
							m0.occlusionMapId = tName;
						}
						else if (texture->flags == TextureType::TEXTURE_NORMAL)
						{
							m0.normalMapId = tName;
						}
						orb->AppendObject(tName, std::move(tex));
					}
				}
                orb->AppendObject(name, std::move(m0));
			}
		}
		for (auto light : m_fbx.lights)
		{
			Light l;
			l._color = light->color;
			l._direction = light->direction;
			l._falloffBegin = light->falloffBegin;
			l._falloffEnd = light->falloffEnd;
			l._position = light->position;
			l._spotAngle = light->spotAngle;
			l._type = (LightType)(uint8_t)light->ltype;
			//orb->AppendObject("", std::move(l));
		}
        
        return true;
    }

    void FbxReader::FBXGeometryAppendToMesh(const FBXGeometry* geometry, OrbMesh* mesh)
    {
        OrbMesh tmpMesh;
		LoadPositions(geometry, &tmpMesh);
		LoadNormals(geometry, &tmpMesh);
		LoadTangents(geometry, &tmpMesh);
		LoadUVs(geometry, &tmpMesh);
		CleanupGeometry(&tmpMesh);

		SubMesh submesh;
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

    void FbxReader::LoadPositions(const FBXGeometry* geometry, OrbMesh* mesh)
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
		std::vector<uint32_t> indices_;
		if (indices.empty())
		{
			indices.resize(positions.size());
			std::iota(indices.begin(), indices.end(), 0);
		}
		else
		{
			indices_.reserve(indices.size());
			// A negative index tells us that we reached the end 
			//	of a face
			bool triangulating = false;
			unsigned faceVertexCount = 0;
			for (auto i = 0u; i < indices.size(); ++i)
			{
				++faceVertexCount;
				auto value = indices[i];
				bool isEnd = value < 0;

				if (isEnd)
					value = std::abs(value) - 1;

				if (faceVertexCount > 3)
				{
					if (!triangulating)
					{
						triangulating = true;
						ORBIT_LOG("Naively triangulating mesh.");
					}

					auto base = i - (faceVertexCount - 1);
					indices_.emplace_back(indices[base]);
					indices_.emplace_back(indices[i - 1]);
					indices_.emplace_back(value);
				}
				else
				{
					indices_.emplace_back(value);
				}

				if (isEnd) // We found a negative index
				{
					if (faceVertexCount >= 4 && m_warnOnQuads)
						ORBIT_THROW("This object file contains n-gons. Triangulate your mesh or use -triangulate.");

					faceVertexCount = 0;
				}
			}
		}

		mesh->indices = std::move(indices_);
		mesh->vertices.resize(positions.size());
		for (auto i = 0u; i < positions.size(); ++i)
			mesh->vertices[i].position = positions[i] * 0.01f;
    }

    void FbxReader::LoadNormals(const FBXGeometry* geometry, OrbMesh* mesh)
    {
        auto& indices = geometry->normals.normalIndices;
		auto& normals = geometry->normals.normals;
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

    void FbxReader::LoadTangents(const FBXGeometry* geometry, OrbMesh* mesh)
    {
        auto& indices = geometry->tangents.tangentIndices;
		auto& tangents = geometry->tangents.tangents;
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

    void FbxReader::LoadUVs(const FBXGeometry* geometry, OrbMesh* mesh)
    {
		/*
		auto& indices = geometry->uvs.uvIndices;
		auto& uvs = geometry->uvs.uvs;
		std::vector<Vector2f> bakedUVs(uvs.size() / 2);
		for (auto i = 0u; i < uvs.size() / 3; ++i)
		{
			auto idx = i * 2;
			bakedUVs[i] = Vector2f{
					static_cast<float>(uvs[idx + 0ll]),
					static_cast<float>(uvs[idx + 1ll])
			};
		}

		auto itd = geometry->uvs.rit == ReferenceInformationType::REFERENCE_INDEX_TO_DIRECT;
		auto mx = itd ? indices.size() : bakedUVs.size();

		for (auto i = 0u; i < mx; ++i)
		{
			auto uv = bakedUVs[itd ? indices[i] : i];
			if (geometry->uvs.mit == MappingInformationType::MAPPING_BY_VERTEX)
				mesh->vertices[i].textureCoords += uv;
			else if (geometry->uvs.mit == MappingInformationType::MAPPING_BY_POLYGON_VERTEX)
				mesh->vertices[mesh->indices[i]].textureCoords += uv;
			else if (geometry->uvs.mit == MappingInformationType::MAPPING_BY_POLYGON)
			{
				mesh->vertices[mesh->indices[i + 0ll]].textureCoords += uv;
				mesh->vertices[mesh->indices[i + 1ll]].textureCoords += uv;
				mesh->vertices[mesh->indices[i + 2ll]].textureCoords += uv;
			}
		}
		*/
        auto tmpVertices = mesh->vertices;
		mesh->vertices.resize(mesh->indices.size());

		for (auto i = 0u; i < mesh->indices.size(); ++i)
			mesh->vertices[i] = tmpVertices[mesh->indices[i]];

		std::iota(mesh->indices.begin(), mesh->indices.end(), 0);

		// Create _positions.size() number of normals
		const auto& indices = geometry->uvs.uvIndices;
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
				mesh->vertices[i].textureCoords = uv;
				vv.emplace_back(i);
			}
			else
			{
				mesh->vertices.emplace_back(mesh->vertices[i]);
				mesh->vertices.back().textureCoords = uv;
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
		//*/
    }

    void FbxReader::CleanupGeometry(OrbMesh* mesh)
    {
		return;
		std::vector<uint32_t> indices;
        std::vector<OrbVertex> mesh_vertices;

        for (const auto& vertex : mesh->vertices)
        {
            auto it = std::find(mesh_vertices.begin(), mesh_vertices.end(), vertex);
            if (it == mesh_vertices.end())
            {
                indices.emplace_back(static_cast<uint32_t>(mesh_vertices.size()));
                mesh_vertices.emplace_back(vertex);
            }
            else
            {
                uint32_t index = it - mesh_vertices.begin();
                indices.emplace_back(index);
            }
        }

        if (mesh_vertices.size() == indices.size())
            indices.clear();

		mesh->vertices = std::move(mesh_vertices);
		mesh->indices = std::move(indices);
    }
    
    Quaterniond FbxReader::QuatFromEuler(Vector3d euler)
    {
        return 
			AngleAxisd(euler.x(), Vector3d::UnitZ()) *
			AngleAxisd(euler.y(), Vector3d::UnitX()) *
			AngleAxisd(euler.z(), Vector3d::UnitZ());
    }

    std::unordered_map<int64_t, std::shared_ptr<FBXInterModel>>::iterator FbxReader::TryInsert(std::shared_ptr<FBXModel> model)
    {
        auto it = m_fbx.models.find(model->id);
		if (it != m_fbx.models.end())
			return it;

		FBXInterModel inter;
		inter.model = model;
		inter.id = model->id;
		inter.type = FBXType::TYPE_INTERMODEL;
		m_fbx.models.emplace(inter.id, std::make_shared<FBXInterModel>(inter));
		return m_fbx.models.find(inter.id);
    }

    MappingInformationType FbxReader::LoadMIT(const FBXNode* mitNode)
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

    ReferenceInformationType FbxReader::LoadRIT(const FBXNode* ritNode)
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

    void FbxReader::LoadFBXData(const FBXNode* root, FBXData* data)
    {
        auto objectsNode = root->FindChild("Objects");
		auto connectionsNode = root->FindChild("Connections");
		if (!objectsNode || !connectionsNode) return;

		LoadModels(objectsNode, data);
		LoadAttributes(objectsNode, data);
		LoadGeometries(objectsNode, data);
		LoadMaterials(objectsNode, data);
		LoadTextures(objectsNode, data);

		LoadConnections(connectionsNode, data);
    }

    void FbxReader::LoadGeometries(const FBXNode* objectsNode, FBXData* data)
    {
        const FBXNode* geometryNode;
		auto idx = 0u;
		while ((geometryNode = objectsNode->FindChild("Geometry", idx++)) != nullptr)
		{
			FBXGeometry fbx_geom;
			fbx_geom.type = FBXType::TYPE_GEOMETRY;
			GetFBXGeometry(&fbx_geom, geometryNode);

			data->nodes.emplace(fbx_geom.id, std::make_shared<FBXGeometry>(std::move(fbx_geom)));
		
		}
    }

    void FbxReader::LoadMaterials(const FBXNode* objectsNode, FBXData* data)
    {
        const FBXNode* materialNode;
		auto idx = 0u;
		while ((materialNode = objectsNode->FindChild("Material", idx++)) != nullptr)
		{
			FBXMaterial fbx_mat;
			fbx_mat.type = FBXType::TYPE_MATERIAL;
			GetFBXMaterial(&fbx_mat, materialNode);

			data->nodes.emplace(fbx_mat.id, std::make_shared<FBXMaterial>(std::move(fbx_mat)));
		}
    }

    void FbxReader::LoadModels(const FBXNode* objectsNode, FBXData* data)
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
		while ((modelNode = objectsNode->FindChild("Model", idx++)) != nullptr)
		{
			FBXModel model;
			model.type = FBXType::TYPE_MODEL;
			model.id = std::get<int64_t>(modelNode->properties[0]);
			model.modelName = std::get<std::string>(modelNode->properties[1]);
			model.modelType = std::get<std::string>(modelNode->properties[2]);
			if (auto p70 = modelNode->FindChild("Properties70"); p70 != nullptr)
			{
				const FBXNode* pNode;
				auto idx0 = 0u;
				while ((pNode = p70->FindChild("P", idx0++)) != nullptr)
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

    void FbxReader::LoadConnections(const FBXNode* connectionsNode, FBXData* data)
    {
        const FBXNode* connectionNode;
		auto idx = 0u;
		while ((connectionNode = connectionsNode->FindChild("C", idx++)) != nullptr)
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

    void FbxReader::LoadTextures(const FBXNode* objectsNode, FBXData* data)
    {
        const FBXNode* textureNode;
		auto idx = 0u;
		while ((textureNode = objectsNode->FindChild("Texture", idx++)) != nullptr)
		{
			FBXTexture texture;
			texture.type = FBXType::TYPE_TEXTURE;
			texture.id = std::get<int64_t>(textureNode->properties[0]);
			texture.name = std::get<std::string>(textureNode->properties[1]);

			if (auto filenameNode = textureNode->FindChild("FileName"); filenameNode != nullptr)
				texture.filepath = std::get<std::string>(filenameNode->properties[0]);
			
			data->nodes.emplace(texture.id, std::make_shared<FBXTexture>(std::move(texture)));
		}
    }

    void FbxReader::LoadAttributes(const FBXNode* objectsNode, FBXData* data)
    {
        const FBXNode* attributeNode;
		auto idx = 0u;
		while ((attributeNode = objectsNode->FindChild("NodeAttribute", idx++)) != nullptr)
		{
			FBXAttribute attribute;
			attribute.type = FBXType::TYPE_ATTRIBUTE;
			attribute.id = std::get<int64_t>(attributeNode->properties[0]);
			attribute.nodeType = std::get<std::string>(attributeNode->properties[1]);
			attribute.nodeName = std::get<std::string>(attributeNode->properties[2]);
			const FBXNode* p70;
			if ((p70 = attributeNode->FindChild("Properties70")); p70 != nullptr)
				attribute.attributes = p70;

			data->nodes.emplace(attribute.id, std::make_shared<FBXAttribute>(std::move(attribute)));
		}
    }

    void FbxReader::GetFBXGeometry(FBXGeometry* geometry, const FBXNode* geometryNode)
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

		verticesNode = geometryNode->FindChild("Vertices");
		indicesNode = geometryNode->FindChild("PolygonVertexIndex");
		normalParent = geometryNode->FindChild("LayerElementNormal");
		tangentParent = geometryNode->FindChild("LayerElementTangent");
		uvParent = geometryNode->FindChild("LayerElementUV");

		if (verticesNode && verticesNode->properties.size() == 1)
			geometry->vertices = std::get<std::vector<double>>(verticesNode->properties[0]);

		if (indicesNode && indicesNode->properties.size() == 1)
			geometry->indices = std::get<std::vector<int>>(indicesNode->properties[0]);

		if (normalParent)
		{
			normalsNode = normalParent->FindChild("Normals");
			normalIndexNode = normalParent->FindChild("NormalIndex");
			mitNode = normalParent->FindChild("MappingInformationType");
			ritNode = normalParent->FindChild("ReferenceInformationType");

			if (normalsNode && normalsNode->properties.size() == 1)
				geometry->normals.normals = std::get<std::vector<double>>(normalsNode->properties[0]);
			if (normalIndexNode && normalIndexNode->properties.size() == 1)
				geometry->normals.normalIndices = std::get<std::vector<int>>(normalIndexNode->properties[0]);

			geometry->normals.mit = LoadMIT(mitNode);
			geometry->normals.rit = LoadRIT(ritNode);
		}

		if (tangentParent)
		{
			tangentNode = tangentParent->FindChild("Tangents");
			tangentIndexNode = tangentParent->FindChild("TangentIndex");
			mitNode = tangentParent->FindChild("MappingInformationType");
			ritNode = tangentParent->FindChild("ReferenceInformationType");

			if (tangentNode && tangentNode->properties.size() == 1)
				geometry->tangents.tangents = std::get<std::vector<double>>(tangentNode->properties[0]);
			if (tangentIndexNode && tangentIndexNode->properties.size() == 1)
				geometry->tangents.tangentIndices = std::get<std::vector<int>>(tangentIndexNode->properties[0]);

			geometry->tangents.mit = LoadMIT(mitNode);
			geometry->tangents.rit = LoadRIT(ritNode);
		}

		if (uvParent)
		{
			uvNode = uvParent->FindChild("UV");
			uvIndexNode = uvParent->FindChild("UVIndex");
			mitNode = uvParent->FindChild("MappingInformationType");
			ritNode = uvParent->FindChild("ReferenceInformationType");

			if (uvNode && uvNode->properties.size() == 1)
				geometry->uvs.uvs = std::get<std::vector<double>>(uvNode->properties[0]);
			if (uvIndexNode && uvIndexNode->properties.size() == 1)
				geometry->uvs.uvIndices = std::get<std::vector<int>>(uvIndexNode->properties[0]);

			geometry->uvs.mit = LoadMIT(mitNode);
			geometry->uvs.rit = LoadRIT(ritNode);
		}
    }

    void FbxReader::GetFBXMaterial(FBXMaterial* material, const FBXNode* materialNode)
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
			auto prop = std::get<std::string>(node->properties[0]);
			prop = prop.substr(0, prop.find('\0'));
			return prop;
		};

		material->id = std::get<int64_t>(materialNode->properties[0]);
		material->name = std::get<std::string>(materialNode->properties[1]);

		auto properties70 = materialNode->FindChild("Properties70");
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
				material->roughness = Sigmoid(static_cast<float>(std::get<double>(p.properties[4])));
			if (channel == "Maya|specularRoughness")
				material->roughness = Sigmoid(std::get<float>(p.properties[4]));
			if (channel == "Shininess")
				material->roughness = Sigmoid(1.f - static_cast<float>(std::get<double>(p.properties[4])));
		}
    }

    void FbxReader::FBXToIntermediate(const FBXData* data)
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
					std::static_pointer_cast<FBXModel>(it1->second));
			else if (it0->second->type == FBXType::TYPE_ATTRIBUTE
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelAttribute(
					std::static_pointer_cast<FBXModel>(it1->second),
					std::static_pointer_cast<FBXAttribute>(it0->second));
			else if (it1->second->type == FBXType::TYPE_ATTRIBUTE
				&& it0->second->type == FBXType::TYPE_MODEL)
				ConnectModelAttribute(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXAttribute>(it1->second));
			else if (it0->second->type == FBXType::TYPE_GEOMETRY
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelGeometry(
					std::static_pointer_cast<FBXModel>(it1->second),
					std::static_pointer_cast<FBXGeometry>(it0->second));
			else if (it1->second->type == FBXType::TYPE_GEOMETRY
				&& it0->second->type == FBXType::TYPE_MODEL)
				ConnectModelGeometry(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXGeometry>(it1->second));
			else if (it0->second->type == FBXType::TYPE_MATERIAL
				&& it1->second->type == FBXType::TYPE_MODEL)
				ConnectModelMaterial(
					std::static_pointer_cast<FBXModel>(it1->second),
					std::static_pointer_cast<FBXMaterial>(it0->second));
			else if (it1->second->type == FBXType::TYPE_MATERIAL
				&& it0->second->type == FBXType::TYPE_MODEL)
				ConnectModelMaterial(
					std::static_pointer_cast<FBXModel>(it0->second),
					std::static_pointer_cast<FBXMaterial>(it1->second));
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

    void FbxReader::ConnectModelModel(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXModel> m1)
    {
        auto im = TryInsert(m0)->second;
		im->children.emplace_back(m1);

		m1->transform.parent = &m0->transform;
    }

    void FbxReader::ConnectModelAttribute(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXAttribute> attr)
    {
        auto im = TryInsert(m0)->second;
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
			m_fbx.lights.emplace_back(std::make_shared<FBXLight>(light));
		}
    }

    void FbxReader::ConnectModelGeometry(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXGeometry> geom)
    {
        auto im = TryInsert(m0)->second;
		im->geometries.emplace_back(geom);
    }

    void FbxReader::ConnectModelMaterial(std::shared_ptr<FBXModel> m0, std::shared_ptr<FBXMaterial> mat)
    {
        auto im = TryInsert(m0)->second;
		im->materials.emplace_back(mat);
    }

    void FbxReader::ConnectMaterialTexture(std::shared_ptr<FBXMaterial> mat, std::shared_ptr<FBXTexture> tex, const std::string& channel)
    {
        tex->flags = TextureType::TEXTURE_OTHER;
		if (channel == "Maya|specularRoughness")
		{
			tex->flags = TextureType::TEXTURE_ROUGHNESS;
		}
		else if (channel == "Maya|normalCamera")
		{
			tex->flags = TextureType::TEXTURE_NORMAL;
		}
		else if (channel == "Maya|baseColor" || channel == "DiffuseColor")
		{
			tex->flags = TextureType::TEXTURE_COLOR;
		}
		mat->textures.emplace_back(tex);
    }

}