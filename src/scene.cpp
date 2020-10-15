#include "scene.hpp"
#include "orb_loader.hpp"
#include "exception.hpp"
#include "engine.hpp"

namespace orbit
{

	std::shared_ptr<Scene> Scene::Create(EnginePtr engine)
	{
		ORBIT_INFO_LEVEL(FormatString("Creating scene."), 10);
		auto scene = std::make_shared<Scene>();
		scene->_engine = engine;
		return scene;
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

		Ptr<ID3D12Resource> intermediateSceneBuffer;
		UpdateBufferResource(
			_engine->GetDevice(),
			cmdList,
			_perFrameBuffer.GetAddressOf(),
			intermediateSceneBuffer.GetAddressOf(),
			1,
			sizeof(ShaderFrameBuffer),
			&buffer
		);

		D3D12_GPU_DESCRIPTOR_HANDLE pfbHandle;
		pfbHandle.ptr = _perFrameBuffer->GetGPUVirtualAddress();

		cmdList->SetGraphicsRootDescriptorTable(0, pfbHandle);
	}

	ObjectPtr Scene::LoadOrb(const fs::path& file, const std::string& objectId)
	{
		ORBIT_INFO_LEVEL(FormatString("Loading orb file '%s'.", file.generic_string().c_str()), 10);

		auto object = _objects.find(objectId);
		if (object == _objects.end())
			return LoadOrb(file, object->second);
		else
		{
			auto obj = std::make_shared<Object>();
			obj->Init(_engine);
			return LoadOrb(file, obj);
		}
	}

	ObjectPtr Scene::LoadOrb(const fs::path& file, ObjectPtr object)
	{
		if (!object) return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Loading orb file '%s'.", file.generic_string().c_str()), 10);

		// Load the corresponding orb file
		OrbLoader loader(_engine);
		loader.LoadOrb(file, this, object);
		return object;
	}

}