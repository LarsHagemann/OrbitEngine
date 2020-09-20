#include "scene.hpp"

namespace orbit
{

	std::shared_ptr<Scene> Scene::create()
	{
		return std::make_shared<Scene>();
	}

	void Scene::AddObject(std::string_view id, ObjectPtr object)
	{
		_objects.emplace(id, object);
	}

	ObjectPtr Scene::FetchObject(std::string_view id) const
	{
		auto obj = _objects.find(std::string(id));
		if (obj == _objects.end()) return nullptr;
		return obj->second;
	}

	void Scene::RemoveObject(std::string_view id)
	{
		_objects.erase(_objects.find(std::string(id)));
	}

	unsigned Scene::AddLight(LightPtr light)
	{
		_lights.emplace(_cachedLightId, light);
		return _cachedLightId++;
	}

	LightPtr Scene::FetchLight(unsigned id) const
	{
		auto light = _lights.find(id);
		if (light == _lights.end()) return nullptr;
		return light->second;
	}

	void Scene::RemoveLight(unsigned id)
	{
		_lights.erase(_lights.find(id));
	}

	void Scene::SetCamera(CameraPtr camera)
	{
		_camera = camera;
	}

	CameraPtr Scene::GetCamera() const
	{
		return _camera;
	}

}