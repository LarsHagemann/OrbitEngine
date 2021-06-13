#pragma once
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Rendering/WindowBase.hpp"
#include "Engine/Bindings/Binding.hpp"
#include "Engine/Management/Allocator.hpp"
#include "Engine/Misc/ThreadPool.hpp"
#include "Engine/Misc/Math.hpp"
#include "Engine/Misc/Time.hpp"
#include "PhysX/PhysXController.hpp"

#include <memory>
#include <unordered_map>

namespace orbit
{

	class Scene;
	class Object;
	class DebugObject;
	using ObjectPtr = std::shared_ptr<Object>;

	class Engine : public EngineResources, public UploadBuffer, public Allocator, public Clock, public PhysXController
	{
	private:
		friend class DebugObject;
		using ObjMap = std::unordered_map<std::string, ObjectPtr>;

		std::shared_ptr<WindowBase> _window;
		std::shared_ptr<Scene> _scene;
		std::shared_ptr<DebugObject> _debugObject;
		std::vector<std::thread> _workerPool;
		std::wstring_view _projectName;
		Vector4f _clearColor = { 0.24f, 0.4f, 0.6f, 1.f };
		Clock _frameClock;
		Time _lastFrametime;

		static std::shared_ptr<Engine> sEngine;
	private:
		void UpdateScene();
		void PartialUpdate(ObjMap::iterator begin, ObjMap::iterator end);
	public:
		static constexpr auto svFOV = Math<float>::PIDIV4;
		static constexpr auto sNearZ = .01f;
		static constexpr auto sFarZ = 20.f;

		Engine(std::shared_ptr<WindowBase> window, InitDesc* desc, std::wstring_view);
		static std::shared_ptr<Engine> Init(
			std::shared_ptr<WindowBase> window, 
			InitDesc* desc,
			std::wstring_view projectName
		);
		static std::shared_ptr<Engine> Get() { return sEngine; }
		std::shared_ptr<WindowBase> GetWindow() const { return _window; }
		void SetScene(std::shared_ptr<Scene> scene) { _scene = scene; }
		void SetDebugObject(std::shared_ptr<DebugObject> debugObject) { _debugObject = debugObject; }
		std::shared_ptr<DebugObject> GetDebugObject() const { return _debugObject; }
		void Run();
		fs::path GetProjectFolder() const;
	};

}
