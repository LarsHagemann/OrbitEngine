#include "Engine/Scene.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Bindings/Binding.hpp"
#include "Engine/Misc/OrbLoader.hpp"

namespace orbit
{

	std::shared_ptr<Scene> Scene::Create()
	{
		ORBIT_INFO_LEVEL(FormatString("Creating scene."), 10);
		return std::make_shared<Scene>();
	}

	void Scene::AddObject(std::string_view id, ObjectPtr object)
	{
		ORBIT_INFO_LEVEL(FormatString("Adding object to scene '%*s'.", id.length(), id.data()), 10);
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

	void Scene::SetAmbientLighting(Vector4f ambient)
	{
		_ambientLight = ambient;
	}

	void Scene::SetCamera(CameraPtr camera)
	{
		_camera = camera;
	}

	CameraPtr Scene::GetCamera() const
	{
		return _camera;
	}

	void Scene::PrepareRendering(Renderer* renderer)
	{
		auto cp = _camera->GetTransform()->GetCombinedTranslation();
		auto bufferWrapper = Engine::Get()->Allocate(sizeof(ShaderFrameBuffer), 256);
		auto buffer = (ShaderFrameBuffer*)bufferWrapper.CPU;
		buffer->scene.numLights = static_cast<uint32_t>(std::min(sMaxSceneLights, _lights.size()));
		buffer->scene.ambientLight = _ambientLight;
		buffer->scene.cameraPosition = Vector4f{ cp.x(), cp.y(), cp.z(), 1.f };
		buffer->projectionMatrix = _camera->GetProjectionMatrix();
		buffer->viewMatrix = _camera->GetViewMatrix();
		auto i = 0u;
		for (const auto& light : _lights)
		{
			buffer->lights[i] = *light.second;
			++i;
			if (i == sMaxSceneLights)
				break;
		}
		
		renderer->BindConstantBuffer(0, bufferWrapper);
	}

	ObjectPtr Scene::LoadOrb(const fs::path& file, const std::string& objectId)
	{
		auto object = _objects.find(objectId);
		if (object != _objects.end())
			return LoadOrb(file, object->second);
		else
		{
			auto obj = std::make_shared<Object>();
			obj->Init();
			AddObject(objectId, obj);
			return LoadOrb(file, obj);
		}
	}

	ObjectPtr Scene::LoadOrb(const fs::path& file, ObjectPtr object)
	{
		if (!object) return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Loading orb file '%s'.", file.generic_string().c_str()), 10);

		// Load the corresponding orb file
		OrbLoader loader;
		if (!loader.LoadOrb(file, this, object))
			ORBIT_WARN(FormatString("Failed to load/open file '%s'!", fs::absolute(file).generic_string().c_str()));
		return object;
	}

}