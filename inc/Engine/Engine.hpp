#pragma once
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Rendering/WindowBase.hpp"
#include "Engine/Bindings/Binding.hpp"
#include "Engine/Management/Allocator.hpp"
#include "Misc/ThreadPool.hpp"
#include "Engine/Misc/Math.hpp"

#include <memory>
#include <unordered_map>

namespace orbit
{

	class Scene;
	class Object;
	using ObjectPtr = std::shared_ptr<Object>;

	class Engine : public EngineResources, public UploadBuffer, public Allocator
	{
	private:
		friend class DebugObject;
		using ObjMap = std::unordered_map<std::string, ObjectPtr>;

		std::shared_ptr<WindowBase> _window;
		std::shared_ptr<Scene> _scene;
		std::shared_ptr<Object> _debugObject;
		ThreadPool _workerPool;
		std::wstring_view _projectName;
		Vector4f _clearColor = { 0.24f, 0.4f, 0.6f, 1.f };

		static std::shared_ptr<Engine> sEngine;
	private:
		void UpdateScene();
		void PartialUpdate(ObjMap::iterator begin, ObjMap::iterator end);
	public:
		static constexpr auto svFOV = Math<float>::PIDIV4;
		static constexpr auto sNearZ = 0.5f;
		static constexpr auto sFarZ = 1000.f;

		Engine(std::shared_ptr<WindowBase> window, InitDesc* desc, std::wstring_view);
		virtual ~Engine() { _workerPool.ForceStopJoin(); }
		static std::shared_ptr<Engine> Init(
			std::shared_ptr<WindowBase> window, 
			InitDesc* desc,
			std::wstring_view projectName
		);
		static std::shared_ptr<Engine> Get() { return sEngine; }
		std::shared_ptr<WindowBase> GetWindow() const { return _window; }
		void SetScene(std::shared_ptr<Scene> scene) { _scene = scene; }
		void SetDebugObject(std::shared_ptr<Object> debugObject) { _debugObject = debugObject; }
		void Run();
		fs::path GetProjectFolder() const;
	};

}
