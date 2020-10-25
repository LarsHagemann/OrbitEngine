#pragma once
#include <filesystem>
#include <fstream>
#include "orb_type.hpp"

namespace orbit
{

	using namespace Eigen;
	namespace fs = std::filesystem;

	class Orb
	{
	protected:
		friend class OrbCompiler;
		std::vector<OrbLight> _lights;
		std::vector<OrbMesh> _meshes;
		std::vector<OrbTexture> _textures;
		std::vector<OrbMaterial> _materials;
	public:
		void AddMesh(OrbMesh&& mesh);
		void AddTexture(OrbTexture&& texture);
		void AddLight(OrbLight&& light);
		void AddMaterial(OrbMaterial&& material);
		void ToFile(const fs::path& file);
	};

	class OrbCompiler
	{
	protected:
		//Orb _orb;
		OrbVersion _version;
		std::ifstream _input;
	protected:
		std::string read_string(size_t strLength);
		uint32_t read_uint32();
		uint16_t read_uint16();
		uint8_t read_byte();

		void read_light();
		void read_material();
		void read_texture();
		void read_geometry();

		void print_light_type(LightType type);
	public:
		void AnalyzeFile(const fs::path& file);
	};

}
