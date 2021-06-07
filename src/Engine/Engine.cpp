#include "Engine/Engine.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Rendering/RenderState.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Misc/Math.hpp"
#include "Engine/Component/Renderable.hpp"
#include "Engine/Component/EventDriven.hpp"
#include "Engine/DebugObject.hpp"

#include <future>

#ifdef ORBIT_WITH_IMGUI
#include "imgui.h"
#ifdef ORBIT_WIN 
#include "imgui_impl_win32.h"
#endif
#ifdef ORBIT_DX12
#include "imgui_impl_dx12.h"
#endif
#endif

#include <string>

namespace orbit
{

	Engine::Engine(std::shared_ptr<WindowBase> window, InitDesc* desc, std::wstring_view projectName) :
		EngineResources(window->GetHandle(), desc),
		_window(window),
		_projectName(projectName),
		_lastFrametime(Time(1))
	{
		

		auto enginePath = GetEngineFolder();
		auto projectPath = enginePath / _projectName;
		if (!fs::exists(projectPath))
		{
			fs::create_directories(projectPath);
			fs::create_directory(projectPath / "shader");
			fs::create_directory(projectPath / "assets");
			fs::copy(enginePath / "__general"_path / "shader"_path, projectPath / "shader"_path);
		}
		fs::current_path(projectPath);
		InitResources();
	}

	std::shared_ptr<Engine> Engine::Init(std::shared_ptr<WindowBase> window, InitDesc* desc, std::wstring_view projectName)
	{
		if (!sEngine)
			sEngine = std::make_shared<Engine>(window, desc, projectName);
		return sEngine;
	}

	void Engine::PartialUpdate(ObjMap::iterator begin, ObjMap::iterator end)
	{
		auto renderer = CreateRenderer();
		auto dimensions = _window->GetDimensions();
		RenderState state;
		state.scissorRect = { 0, 0, dimensions.x(), dimensions.y() };
		state.viewport = { 0, 0, static_cast<float>(dimensions.x()), static_cast<float>(dimensions.y()), 0.f, 1.f };
		renderer.PrepareRenderState(state);
		renderer.BindRootSignature("orbit/default");
		renderer.BindPipelineState("orbit/default");
		_scene->PrepareRendering(&renderer);
		std::for_each(begin, end, [&](const auto& obj_pair) {
			auto object = obj_pair.second;
			if (!object->IsActive()) return;

			for (auto component : object->_components)
			{
				// change this to flag lookups:
				auto rndComp = std::dynamic_pointer_cast<Renderable>(component.second);
				auto evntComp = std::dynamic_pointer_cast<EventDriven>(component.second);
				if (rndComp) rndComp->Draw(&renderer);
				if (evntComp) evntComp->Update(_lastFrametime);
			}

			object->Update(_lastFrametime);
		});
	}

	void Engine::UpdateScene()
	{
		auto renderer = GetDefaultRenderer();
		renderer.BindRootSignature("orbit/default");
		renderer.BindPipelineState("orbit/default");
		_scene->PrepareRendering(&renderer);

		_lastFrametime = _frameClock.Restart();

//#ifdef _DEBUG
		_debugObject->GetStatic<KeyboardComponent>("keyboard_handler")->Update(_lastFrametime);
		_debugObject->Update(_lastFrametime);
//#endif

		std::vector<std::future<void>> results;

		const auto workPerThread = std::max(static_cast<size_t>(1u), _scene->_objects.size() / GetParallelRenderCount());
		for (auto i = 0U; i < GetParallelRenderCount(); ++i)
		{
			if (i * workPerThread >= _scene->_objects.size())
				break;
			
			auto beginIdx = i * workPerThread;
			auto endIdx =
				(i + 1) * workPerThread < _scene->_objects.size()
				? (i + 1) * workPerThread
				: _scene->_objects.size();

			auto begin = _scene->_objects.begin();
			auto end = _scene->_objects.begin();

			std::advance(begin, beginIdx);
			std::advance(end, endIdx);

			results.emplace_back(std::async(&Engine::PartialUpdate, this, begin, end));
		}

		for (auto i = 0U; i < results.size(); ++i)
			results.at(i).wait();
	}

	void Engine::Run()
	{
		ORBIT_INFO_LEVEL(FormatString("Running engine."), 5);
		OnResize();
		auto size = _window->GetDimensions();
		auto aspect = static_cast<float>(size.x()) / size.y();
		if (_scene && _scene->GetCamera())
			_scene->GetCamera()->RecalculateProjectionMatrix(svFOV, aspect, sNearZ, sFarZ);

//#ifdef _DEBUG
		_debugObject = std::make_shared<DebugObject>();
		_debugObject->Init();
//#endif

		_frameClock.Restart();

		while (_window->IsOpen())
		{
			_window->HandleEvents();
			Clear(_clearColor);
			UpdateScene();
			Display();

			UploadBuffer::Reset();

			if (_window->_resizeNecessary)
			{
				_window->_resizeNecessary = false;
				auto size = _window->GetDimensions();
				auto aspect = static_cast<float>(size.x()) / size.y();
				if (_scene && _scene->GetCamera())
					_scene->GetCamera()->RecalculateProjectionMatrix(svFOV, aspect, sNearZ, sFarZ);
				OnResize();
			}
		}

		//_workerPool.ForceStopJoin();
		Cleanup();

		ORBIT_INFO_LEVEL(FormatString("Shutting down."), 5);

#ifdef ORBIT_WITH_IMGUI
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif
	}

	fs::path Engine::GetProjectFolder() const
	{
		return GetEngineFolder() / _projectName;
	}

	std::shared_ptr<Engine> Engine::sEngine = nullptr;

}