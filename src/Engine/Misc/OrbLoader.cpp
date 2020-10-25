#include "Engine/Misc/OrbLoader.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Object.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Rendering/Light.hpp"
#include "Engine/Rendering/Mesh.hpp"
#include "Engine/Component/BatchComponent.hpp"
#include "Engine/Rendering/Material.hpp"
#include "Engine/Rendering/Texture.hpp"

#include "zlib/zlib.h"

#include <sstream>

namespace orbit
{

	bool OrbLoader::LoadOrb(const fs::path& orb_path, Scene* scene, ObjectPtr object)
	{
		_scene = scene;
		_object = object;

		_input.open(orb_path, std::ios::binary | std::ios::in);
		if (!_input.is_open()) return false;
		
#ifdef _DEBUG
		ORBIT_INFO(FormatString("Reading orb file '%s'.", fs::absolute(orb_path).generic_string().c_str()));
#endif

		constexpr auto headerSize = 4;

		auto header = read_string(headerSize);
		auto _header = std::string("OFF") + '\0';
		if (header != _header)
			throw "File header incorrect";

		_version.high._v1 = read_uint32();
		_version.build_type = read_byte();
		_version.build = read_byte();
		read_uint16(); // unused bytes

		if (_version.high._v0.major < 3) 
			throw std::exception("File format version too old! Cannot load.");

#ifdef _DEBUG
		std::ostringstream iss;
		iss << _version;
		ORBIT_INFO_LEVEL(FormatString("Orbit file version: %s", iss.str().c_str()), 15);
#endif

		auto numLights = read_uint16();
		auto numMaterials = read_uint16();
		auto numTextures = read_uint16();
		auto numGeometries = read_uint16();
		auto numAnimations = read_uint16();

#ifdef _DEBUG
		ORBIT_INFO_LEVEL(FormatString("Number of lights    : %d", numLights), 18);
		ORBIT_INFO_LEVEL(FormatString("Number of materials : %d", numMaterials), 18);
		ORBIT_INFO_LEVEL(FormatString("Number of textures  : %d", numTextures), 18);
		ORBIT_INFO_LEVEL(FormatString("Number of meshes    : %d", numGeometries), 18);
		ORBIT_INFO_LEVEL(FormatString("Number of animations: %d", numAnimations), 18);
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
		return true;
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

		Material material(mData);
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_COLOR_MAP) == (uint8_t)MaterialFlag::F_HAS_COLOR_MAP)
			material.SetTexture(TextureType::T_COLOR, read_string(read_byte()));
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP) == (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP)
			material.SetTexture(TextureType::T_NORMAL, read_string(read_byte()));
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP) == (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP)
			material.SetTexture(TextureType::T_ROUGHNESS, read_string(read_byte()));
		if ((mData.flags & (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP) == (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP)
			material.SetTexture(TextureType::T_OCCLUSION, read_string(read_byte()));

		Engine::Get()->AddMaterial(name, std::make_shared<Material>(material));
	}

	void OrbLoader::read_texture()
	{
		Texture texture;
		auto name = read_string(read_byte());
		auto dataSize = read_uint32();
		std::vector<uint8_t> bytes;
		bytes.resize(dataSize);
		_input.read((char*)bytes.data(), dataSize);
		if (texture.LoadFromBytes(bytes))
			Engine::Get()->AddTexture(name, std::make_shared<Texture>(texture));
	}

	void OrbLoader::read_geometry()
	{
		Mesh mesh;
		auto name = read_string(read_byte());
		auto numVertices = read_uint32();
		auto compressedSize = read_uint32();
		auto stride = read_byte();

		uLongf fullsize = static_cast<unsigned>(stride) * numVertices;
		auto buffer = Engine::Get()->CPUAllocate(fullsize);
		auto compressed = Engine::Get()->CPUAllocate(compressedSize);

		_input.read((char*)compressed.memory, compressed.size);
		uncompress((Bytef*)buffer.memory, &fullsize, (Bytef*)compressed.memory, (uLongf)compressed.size);
		compressed.Free();

		mesh._vertices.SetBuffer(buffer, stride, numVertices);
		mesh._indices.is16Bit = numVertices < std::numeric_limits<uint16_t>::max();

		auto numIndices = read_uint32();
		compressedSize = read_uint32();
		if (numIndices > 0)
		{
			fullsize = numIndices * ((mesh._indices.is16Bit) ? 1 : 2) * sizeof(uint16_t);
			mesh._indices.indices.resize(fullsize / sizeof(uint16_t));
			compressed = Engine::Get()->CPUAllocate(compressedSize);

			_input.read((char*)compressed.memory, compressed.size);
			uncompress((Bytef*)mesh._indices.indices.data(), &fullsize, (Bytef*)compressed.memory, (uLongf)compressed.size);
			compressed.Free();
		}

		mesh.ReloadBuffer();

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
			auto bc = _object->AddComponent<BatchComponent>(name, std::make_shared<Mesh>(mesh));
	}

	OrbLoader::OrbLoader() : 
		_scene(nullptr),
		_version({ 0, 0 })
	{
	}

}