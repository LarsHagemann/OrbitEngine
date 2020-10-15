#pragma once
#include <memory>
#include <fstream>
#include <filesystem>

namespace orbit
{

	namespace fs = std::filesystem;

	class Scene;
	class Object;
	class Engine;
	using ObjectPtr = std::shared_ptr<Object>;
	using EnginePtr = std::shared_ptr<Engine>;

	union OrbVersion {
		struct {
			uint16_t major;
			uint16_t revision;
		} _v0;
		uint32_t _v1;
	};

	class OrbLoader
	{
	protected:
		// @member: input file
		std::ifstream _input;
		// @member: the version read from the input file
		OrbVersion _version;
		// @member: the current scene
		Scene* _scene;
		// @member: the current object
		ObjectPtr _object;
		// @member: game engine instance
		EnginePtr _engine;
	protected:
		// @method: reads 4 bytes from the current file
		uint32_t read_uint32();
		// @method: reads 2 bytes from the current file
		uint16_t read_uint16();
		// @method: reads a single byte from the current file
		uint8_t read_byte();
		// @method: reads a string from the current file
		std::string read_string(unsigned length);

		// @method: reads a light from the current file
		void read_light();
		// @method: reads a material from the current file
		void read_material();
		// @method: reads a texture from the current file
		void read_texture();
		// @method: reads a geometry from the current file
		void read_geometry();
	public:
		// @constructor
		OrbLoader(EnginePtr engine);
		// @method: loads an orb file into a scene or an object
		// @param orb_path: the orb file to be loaded
		// @param scene: the scene to load lights into
		// @param object: the object to associate meshes with
		// @return: status of the orb reading
		bool LoadOrb(const fs::path& orb_path, Scene* scene, ObjectPtr object);
	};

}
