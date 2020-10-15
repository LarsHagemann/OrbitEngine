#include "orb_loader.hpp"
#include "scene.hpp"
#include "object.hpp"
#include "exception.hpp"
#include "light.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "mesh.hpp"
#include "engine.hpp"
#include "batch_component.hpp"

#include "zlib.h"

namespace orbit
{

	bool OrbLoader::LoadOrb(const fs::path& orb_path, Scene* scene, ObjectPtr object)
	{
		_scene = scene;
		_object = object;

		_input.open(orb_path, std::ios::binary | std::ios::in);
		if (!_input.is_open()) return false;

#ifdef _DEBUG
		ORBIT_INFO(FormatString("Reading orb file '%s'.", orb_path.generic_string().c_str()));
#endif

		constexpr auto headerSize = 4;

		auto header = read_string(headerSize);
		auto _header = std::string("OFF") + '\0';
		if (header != _header)
			throw "File header incorrect";

		_version._v1 = read_uint32();

		auto numLights = read_uint16();
		auto numMaterials = read_uint16();
		auto numTextures = read_uint16();
		auto numGeometries = read_uint16();
		auto numAnimations = read_uint16();

#ifdef _DEBUG
		ORBIT_INFO(FormatString("Number of lights    : %d", numLights));
		ORBIT_INFO(FormatString("Number of materials : %d", numMaterials));
		ORBIT_INFO(FormatString("Number of textures  : %d", numTextures));
		ORBIT_INFO(FormatString("Number of meshes    : %d", numGeometries));
		ORBIT_INFO(FormatString("Number of animations: %d", numAnimations));
#endif

		for (auto i = 0u; i < numLights; ++i)
			read_light();
		for (auto i = 0u; i < numMaterials; ++i)
			read_material();
		for (auto i = 0u; i < numTextures; ++i)
			read_texture();
		for (auto i = 0u; i < numGeometries; ++i)
			read_geometry();
		//for (auto i = 0u; i < numAnimations; ++i)
		//	read_animation();
	}

	uint32_t OrbLoader::read_uint32()
	{
		uint32_t i;
		_input.read((char*)&i, sizeof(uint32_t));
		return i;
	}

	uint16_t OrbLoader::read_uint16()
	{
		uint16_t i;
		_input.read((char*)&i, sizeof(uint16_t));
		return i;
	}

	uint8_t OrbLoader::read_byte()
	{
		uint8_t i;
		_input.read((char*)&i, sizeof(uint8_t));
		return i;
	}

	std::string OrbLoader::read_string(unsigned length)
	{
		std::string str;
		str.resize(length);
		_input.read((char*)str.data(), length);
		return str;
	}

	void OrbLoader::read_light()
	{
		Light light;
		_input.read((char*)&light, sizeof(Light));
		_scene->AddLight(std::make_shared<Light>(light));
	}

	void OrbLoader::read_material()
	{
		auto name = read_string(read_byte());
		MaterialData mData;
		_input.read((char*)&mData, sizeof(MaterialData));

		Material material(_engine, mData);
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_COLOR_MAP) == (uint8_t)MaterialFlag::F_HAS_COLOR_MAP)
			material.SetTexture(TextureType::T_COLOR, read_string(read_byte()));
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP) == (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP)
			material.SetTexture(TextureType::T_NORMAL, read_string(read_byte()));
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP) == (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP)
			material.SetTexture(TextureType::T_ROUGHNESS, read_string(read_byte()));
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP) == (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP)
			material.SetTexture(TextureType::T_OCCLUSION, read_string(read_byte()));

		_engine->AddMaterial(name, std::make_shared<Material>(material));
	}

	void OrbLoader::read_texture()
	{
		Texture texture;
		auto name = read_string(read_byte());
		auto dataSize = read_uint32();
		std::vector<uint8_t> bytes;
		bytes.resize(dataSize);
		_input.read((char*)bytes.data(), dataSize);
		if (texture.LoadFromBytes(bytes, _engine->GetDevice()))
			_engine->AddTexture(name, std::make_shared<Texture>(texture));
	}

	void OrbLoader::read_geometry()
	{
		Mesh mesh;
		auto name = read_string(read_byte());
		auto numVertices = static_cast<unsigned>(read_uint32());
		mesh._vertices.resize(numVertices);
		auto compressedSize = static_cast<unsigned>(read_uint32());
		uLongf fullSize = numVertices * sizeof(Vertex);
		std::vector<uint8_t> rawGeometry(compressedSize);
		_input.read((char*)rawGeometry.data(), compressedSize);
		uncompress((Bytef*)mesh._vertices.data(), &fullSize, (Bytef*)rawGeometry.data(), compressedSize);
		auto numSubmeshes = read_uint32();
		for (auto i = 0u; i < numSubmeshes; ++i)
		{
			SubMesh submesh;
			submesh.startVertex = read_uint32();
			submesh.vertexCount = read_uint32();
			submesh.startIndex = read_uint32();
			submesh.indexCount = read_uint32();
			submesh.material = read_string(read_byte());
			submesh.pipelineState = "orbit/default";

			mesh._submeshes.emplace_back(std::move(submesh));
		}
		
		if (_object)
			auto bc = _object->AddComponent<BatchComponent>(name, _engine, std::make_shared<Mesh>(mesh));

	}

	OrbLoader::OrbLoader(EnginePtr engine) : 
		_engine(engine)
	{
	}

}