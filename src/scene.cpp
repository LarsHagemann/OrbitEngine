#include "scene.hpp"

namespace orbit
{

	std::shared_ptr<Scene> Scene::Create()
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

	void Scene::PrepareRendering(Ptr<ID3D12GraphicsCommandList> cmdList)
	{
		auto cp = _camera->GetTransform()->GetCombinedTranslation();
		ShaderFrameBuffer buffer;
		buffer.scene.numLights = std::min(sMaxSceneLights, _lights.size());
		buffer.scene.ambientLight = _ambientLight;
		buffer.scene.cameraPosition = Vector4f{ cp.x(), cp.y(), cp.z(), 1.f };
		buffer.projectionMatrix = _camera->GetProjectionMatrix();
		buffer.viewMatrix = _camera->GetViewMatrix();
		auto i = 0u;
		for (const auto& light : _lights)
		{
			buffer.lights[i] = *light.second;
			++i;
			if (i == sMaxSceneLights)
				break;
		}

		void* cpu_buffer;
		//if (SUCCEEDED(_perFrameBuffer->Map(0, nullptr, &cpu_buffer)))
		//{
		//	memcpy_s(cpu_buffer, sizeof(ShaderFrameBuffer), &buffer, sizeof(ShaderFrameBuffer));
		//	_perFrameBuffer->Unmap(0, nullptr);
		//}


	}

}