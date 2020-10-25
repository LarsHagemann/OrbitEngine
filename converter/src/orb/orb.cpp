#include "orb/orb.hpp"
#include "zlib/zlib.h"
#include <iostream>

namespace orbit
{

	void Orb::AddMesh(OrbMesh&& mesh)
	{
		for (auto& m : _meshes)
			if (m.name == mesh.name)
				return; // mesh already in the orb

		_meshes.emplace_back(std::move(mesh));
	}

	void Orb::AddTexture(OrbTexture&& texture)
	{
		for (auto& tex : _textures)
			if (tex.name == texture.name)
				return; // texture already in the orb

		_textures.emplace_back(std::move(texture));
	}

	void Orb::AddLight(OrbLight&& light)
	{
		_lights.emplace_back(std::move(light));
	}

	void Orb::AddMaterial(OrbMaterial&& material)
	{
		for (auto& m0 : _materials)
			if (m0.name == material.name)
				return; // material already in the orb

		_materials.emplace_back(std::move(material));
	}

	void Orb::ToFile(const fs::path& file)
	{
		std::ofstream output(file, std::ios::binary | std::ios::out);
		if (!output.good()) return;

		const auto pow2_16 = static_cast<unsigned>(std::pow(2, 16));
		const auto pow2_8 = static_cast<unsigned>(std::pow(2, 8));

		auto write_byte_string = [&](const std::string& str)
		{
			auto tmp = str.substr(0, pow2_8);
			if (tmp.size() != str.size())
				printf_s("String to long (max 255 bytes) ['%s' => '%s']", str.data(), tmp.data());

			auto size = static_cast<uint8_t>(tmp.size());
			output.write((char*)&size, 1);
			output.write(tmp.data(), tmp.size());
		};

		const auto rawNumLights = static_cast<uint32_t>(_lights.size());
		const auto rawNumMaterials = static_cast<uint32_t>(_materials.size());
		const auto rawNumTextures = static_cast<uint32_t>(_textures.size());
		const auto rawNumGeometries = static_cast<uint32_t>(_meshes.size());
		const auto rawNumAnimations = static_cast<uint32_t>(0u);

		const auto numLights = static_cast<uint16_t>(std::min(pow2_16, rawNumLights));
		const auto numMaterials = static_cast<uint16_t>(std::min(pow2_16, rawNumMaterials));
		const auto numTextures = static_cast<uint16_t>(std::min(pow2_16, rawNumTextures));
		const auto numGeometries = static_cast<uint16_t>(std::min(pow2_16, rawNumGeometries));
		const auto numAnimations = static_cast<uint16_t>(std::min(pow2_16, rawNumAnimations));

		if (auto diff = std::abs<unsigned>(rawNumLights - numLights); diff != 0)
			printf_s("[WARNING]: Too many lights. Ignoring %d lights\n", diff);
		if (auto diff = std::abs<unsigned>(numMaterials - rawNumMaterials); diff != 0)
			printf_s("[WARNING]: Too many materials. Ignoring %d materials\n", diff);
		if (auto diff = std::abs<unsigned>(numTextures - rawNumTextures); diff != 0)
			printf_s("[WARNING]: Too many textures. Ignoring %d textures\n", diff);
		if (auto diff = std::abs<unsigned>(numGeometries - rawNumGeometries); diff != 0)
			printf_s("[WARNING]: Too many geometries. Ignoring %d geometries\n", diff);
		if (auto diff = std::abs<unsigned>(numAnimations - rawNumAnimations); diff != 0)
			printf_s("[WARNING]: Too many animations. Ignoring %d animations\n", diff);

		// WRITE HEADER DATA
		output.write("OFF", 4);
		output.write((char*)&tool_version, 8);
		output.write((char*)&numLights, 2);
		output.write((char*)&numMaterials, 2);
		output.write((char*)&numTextures, 2);
		output.write((char*)&numGeometries, 2);
		output.write((char*)&numAnimations, 2);
		
		// WRITE LIGHT DATA
		output.write((char*)_lights.data(), sizeof(OrbLight) * static_cast<size_t>(numLights));

		// WRITE MATERIAL DATA
		for (const auto& material : _materials)
		{
			write_byte_string(material.name);
			auto padding = Vector2f::Zero();

			// MATERIAL DATA:
			output.write((char*)&material.diffuse, sizeof(Vector4f));
			output.write((char*)&material.roughness, sizeof(float));
			output.write((char*)&material.flags, sizeof(uint32_t));
			output.write((char*)&padding, sizeof(Vector2f));

			if ((material.flags & (uint8_t)MaterialFlag::F_HAS_COLOR_MAP) == (uint8_t)MaterialFlag::F_HAS_COLOR_MAP)
				write_byte_string(material.colorTextureId);
			if ((material.flags & (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP) == (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP)
				write_byte_string(material.normalTextureId);
			if ((material.flags & (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP) == (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP)
				write_byte_string(material.roughnessTextureId);
			if ((material.flags & (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP) == (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP)
				write_byte_string(material.occlusionTextureId);
		}

		// WRITE TEXTURE DATA
		for (const auto& texture : _textures)
		{
			std::ifstream textureFile(texture.file.filename(), std::ios::binary | std::ios::in | std::ios::ate);
			if (!textureFile.is_open())
			{
				std::ostringstream oss;
				oss << "Fatal Error: Unable to open texture file: " << texture.file.filename();
				throw std::exception(oss.str().c_str());
			}

			write_byte_string(texture.name);

			// store texture inplace
			auto size = (uint32_t)textureFile.tellg();
			output.write((char*)&size, 4);
			textureFile.seekg(0, std::ios::beg);
			std::vector<uint8_t> data;
			data.resize(size);
			textureFile.read((char*)data.data(), size);
			output.write((const char*)data.data(), size);
		}

		// WRITE GEOMETRY DATA
		for (const auto& geometry : _meshes)
		{
			write_byte_string(geometry.name);
			auto vCount = geometry.vertices.size();
            constexpr auto stride = static_cast<uint8_t>(sizeof(Vertex));
			uLongf vSize = static_cast<unsigned long>(vCount * sizeof(Vertex));
			std::vector<uint8_t> rawGeometry(vSize);
			compress((Bytef*)rawGeometry.data(), &vSize, (Bytef*)geometry.vertices.data(), vSize);
			output.write((char*)&vCount, 4);
			output.write((char*)&vSize, 4);
            output.write((char*)&stride, sizeof(uint8_t));
			output.write((char*)rawGeometry.data(), vSize);

			auto iCount = geometry.indices.size();
			uLongf iSize = static_cast<unsigned long>(iCount * sizeof(uint16_t));
			std::vector<uint8_t> rawIndices(iSize);
			compress((Bytef*)rawIndices.data(), &iSize, (Bytef*)geometry.indices.data(), iSize);
			output.write((char*)&iCount, 4);
			output.write((char*)&iSize, 4);
			output.write((char*)rawIndices.data(), iSize);

			// WRITE SUBMESHES DATA:
			auto numSubmeshes = (uint32_t)geometry.submeshes.size();
			output.write((char*)&numSubmeshes, sizeof(uint32_t));
			for (const auto& submesh : geometry.submeshes)
			{
				output.write((char*)&submesh.startVertex, sizeof(uint32_t));
				output.write((char*)&submesh.vertexCount, sizeof(uint32_t));
				output.write((char*)&submesh.startIndex, sizeof(uint32_t));
				output.write((char*)&submesh.indexCount, sizeof(uint32_t));

				write_byte_string(submesh.material);
			}
		}

		// WRITE ANIMATION DATA

		output.close();
	}

	std::string OrbCompiler::read_string(size_t strLength)
	{
		std::string str;
		str.resize(strLength);
		_input.read(str.data(), strLength);
		return str;
	}

	uint32_t OrbCompiler::read_uint32()
	{
		uint32_t tmp;
		_input.read((char*)&tmp, sizeof(uint32_t));
		return tmp;
	}

	uint16_t OrbCompiler::read_uint16()
	{
		uint16_t tmp;
		_input.read((char*)&tmp, sizeof(uint16_t));
		return tmp;
	}

	uint8_t OrbCompiler::read_byte()
	{
		uint8_t tmp;
		_input.read((char*)&tmp, 1);
		return tmp;
	}

	void OrbCompiler::read_light()
	{
		OrbLight light;
		_input.read((char*)&light, sizeof(OrbLight));

		printf_s(
			"Lighttype             : ");
		print_light_type(light._type);
		printf_s(
			"\nLight strength        : { %f, %f, %f, %f }\n"
			"Light position        : { %f, %f, %f, %f }\n"
			"Light direction       : { %f, %f, %f, %f }\n"
			"Light angle           : %f\n"
			"Light falloff begin   : %f\n"
			"Light falloff end     : %f\n\n",
			light._color.x(), light._color.y(), light._color.z(), light._color.w(),
			light._position.x(), light._position.y(), light._position.z(), light._position.w(),
			light._direction.x(), light._direction.y(), light._direction.z(), light._direction.w(),
			light._spotAngle,
			light._falloffBegin,
			light._falloffEnd
		);
	}

	void OrbCompiler::read_material()
	{
		OrbMaterial material;
		material.name = read_string(read_byte());
		_input.read((char*)&material.diffuse, sizeof(Vector4f));
		_input.read((char*)&material.roughness, sizeof(float));
		material.flags = read_uint32();
		Vector2f padding;
		_input.read((char*)&padding, sizeof(Vector2f));
		if ((material.flags & (uint8_t)MaterialFlag::F_HAS_COLOR_MAP) == (uint8_t)MaterialFlag::F_HAS_COLOR_MAP)
			material.colorTextureId = read_string(read_byte());
		if ((material.flags & (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP) == (uint8_t)MaterialFlag::F_HAS_NORMAL_MAP)
			material.normalTextureId = read_string(read_byte());
		if ((material.flags & (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP) == (uint8_t)MaterialFlag::F_HAS_ROUGHNESS_MAP)
			material.roughnessTextureId = read_string(read_byte());
		if ((material.flags & (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP) == (uint8_t)MaterialFlag::F_HAS_OCCLUSION_MAP)
			material.occlusionTextureId = read_string(read_byte());

		printf_s(
			"Material name         : %s\n"
			"Material diffuse color: { %f, %f, %f, %f }\n"
			"Material roughness    : %f\n"
			"Material flags        : %d\n\n",
			material.name.data(),
			material.diffuse.x(), material.diffuse.y(), material.diffuse.z(), material.diffuse.w(),
			material.roughness,
			material.flags
		);
	}

	void OrbCompiler::read_texture()
	{
		OrbTexture texture;
		texture.name = read_string(read_byte());
		auto dataSize = read_uint32();
		std::vector<uint8_t> bytes;
		bytes.resize(dataSize);
		_input.read((char*)bytes.data(), dataSize);
		
		printf_s(
			"Texture name          : %s\n"
			"Texture size          : %d\n",
			texture.name.c_str(),
			dataSize
		);
		printf_s("\n");
	}

	void OrbCompiler::read_geometry()
	{
		OrbMesh mesh;
		mesh.name = read_string(read_byte());
		auto numVertices = static_cast<unsigned>(read_uint32());
		mesh.vertices.resize(numVertices);
		auto compressedSize = static_cast<unsigned>(read_uint32());
		auto stride = read_byte();
		uLongf fullSize = numVertices * stride;
		std::vector<uint8_t> rawGeometry(compressedSize);
		_input.read((char*)rawGeometry.data(), compressedSize);
		uncompress((Bytef*)mesh.vertices.data(), &fullSize, (Bytef*)rawGeometry.data(), compressedSize);

		auto numIndices = read_uint32();
		mesh.indices.resize(numIndices);
		auto compressedIndexSize = read_uint32();
		fullSize = numIndices * sizeof(uint32_t);
		std::vector<uint32_t> rawIndices(compressedIndexSize);
		_input.read((char*)rawIndices.data(), compressedIndexSize);
		uncompress((Bytef*)mesh.indices.data(), &fullSize, (Bytef*)rawIndices.data(), compressedIndexSize);

		auto numSubmeshes = read_uint32();
		for (auto i = 0u; i < numSubmeshes; ++i)
		{
			SubMesh submesh;
			submesh.startVertex = read_uint32();
			submesh.vertexCount = read_uint32();
			submesh.startIndex = read_uint32();
			submesh.indexCount = read_uint32();
			submesh.material = read_string(read_byte());

			mesh.submeshes.emplace_back(std::move(submesh));
		}
		
		printf_s("Geometry name    : %*s\n", static_cast<unsigned>(mesh.name.size()), mesh.name.c_str());
		printf_s("Vertices         : %zd\n", mesh.vertices.size());
		printf_s("Vertex stride    : %d\n", stride);
		printf_s("Indices          : %zd\n", mesh.indices.size());
		printf_s("Submeshes        : %zd\n", mesh.submeshes.size());
		auto i = 0U;
		for (auto& submesh : mesh.submeshes)
		{
			printf_s("\tSubmesh[%d]\n", i);
			printf_s("\tVertex count: %d\n", submesh.vertexCount);
			printf_s("\tIndex count : %d\n", submesh.indexCount);
			printf_s("\tVertex start: %d\n", submesh.startVertex);
			printf_s("\tIndex start : %d\n", submesh.startIndex);
			++i;
		}
		printf_s("\n");
	}

	void OrbCompiler::print_light_type(LightType type)
	{
		switch (type)
		{
		case orbit::LightType::DirectionalLight: printf_s("Directional");
			break;
		case orbit::LightType::PointLight: printf_s("Point");
			break;
		case orbit::LightType::SpotLight: printf_s("Spot");
			break;
		}
	}

	void OrbCompiler::AnalyzeFile(const fs::path& file)
	{
		constexpr auto headerSize = 4;

		_input.open(file, std::ios::binary | std::ios::in);
		if (!_input.is_open() || !_input.good())
			throw "Unable to open file";

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
		if (_version.high._v0.major == 3 && _version.high._v0.revision <= 2)
			throw std::exception("File format version too old! Cannot load.");

		std::cout << "Orbit file compiled with orbtool " << _version << "\n\n";

		auto numLights = read_uint16();
		auto numMaterials = read_uint16();
		auto numTextures = read_uint16();
		auto numGeometries = read_uint16();
		auto numAnimations = read_uint16();

		printf_s(
			"Number of lights    : %d\n"
			"Number of materials : %d\n"
			"Number of textures  : %d\n"
			"Number of meshes    : %d\n"
			"Number of animations: %d\n\n",
			numLights,
			numMaterials,
			numTextures,
			numGeometries,
			numAnimations
		);

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

}