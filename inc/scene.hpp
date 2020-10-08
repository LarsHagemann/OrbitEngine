#pragma once
#include "object.hpp"
#include "helper.hpp"
#include "light.hpp"
#include "base_camera.hpp"

#include <unordered_map>

namespace orbit
{

	// @brief: A game (most likely) consists of several scenes
	//	that you can imagine being seperate stage designs that can
	//	be swapped fairly fast
	class Scene
	{
	protected:
		friend class Engine;
		// @member: the objects in the scene (the player, enemies, terrain, etc...)
		std::unordered_map<std::string, ObjectPtr> _objects;
		// @member: the lights in the scene
		std::unordered_map<unsigned, LightPtr> _lights;
		// @member: id of the next light to be inserted
		unsigned _cachedLightId = 0;
		// @member: camera rendering the scene
		CameraPtr _camera;
	public:
		// @method: creates a new scene object
		static std::shared_ptr<Scene> create();
		// @method: adds an object to the scene
		// @param id: the identifier for the object
		// @param object: the object to be added
		void AddObject(std::string_view id, ObjectPtr object);
		// @method: fetches an object from the scene
		// @param id: the identifier of the requested object
		// @return: the object (or nullptr if the object does not exist)
		ObjectPtr FetchObject(std::string_view id) const;
		// @method: removes an object from the scene
		// @param id: the identifier of the object to be removed
		// @note: expects the object to exist
		void RemoveObject(std::string_view id);
		// @method: adds a light to the scene
		// @param light: the light to be added
		// @return: the light's id
		unsigned AddLight(LightPtr light);
		// @method: fetches a light from the scene
		// @param id: the identifier of the requested light
		// @return: the light (or nullptr if the light does not exist)
		LightPtr FetchLight(unsigned id) const;
		// @method: removes a light from the scene
		// @param id: the identifier of the light to be removed
		// @note: expects the light to exist
		void RemoveLight(unsigned id);
		// @method: sets the scene's rendering camera
		// @param camera: the new scene camera
		void SetCamera(CameraPtr camera);
		// @method: returns the scene's camera
		CameraPtr GetCamera() const;
	};

	using ScenePtr = std::shared_ptr<Scene>;

}
