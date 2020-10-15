#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "Orbit.lib")

#include <iostream>
#include "engine.hpp"
#include "third_person_camera.hpp"
#include "keyboard_component.hpp"
#include "imgui.h"

using namespace orbit;

class SimpleWindowObject : public Object
{
protected:
	std::shared_ptr<KeyboardComponent> _kHandler;
public:
	virtual void Init(EnginePtr engine) override
	{
		Object::Init(engine);
		_kHandler = AddComponent<KeyboardComponent>("keyboard_controller", _engine);
	}

	virtual void Update(Time dt) override
	{
		if (_kHandler->keydownThisFrame(DIK_ESCAPE))
			_engine->CloseWindow();
		if (_kHandler->keydownThisFrame(DIK_F11))
			_engine->SetFullscreen(!_engine->GetFullscreen());

		ImGuiWindowFlags flags = 
			ImGuiWindowFlags_NoMove | 
			ImGuiWindowFlags_NoCollapse | 
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoDecoration;

		static std::vector<float> frametimes;
		frametimes.emplace_back(static_cast<float>(dt.asSeconds()));

		if (frametimes.size() > 240)
			frametimes.erase(frametimes.begin());

		auto max = *std::max_element(frametimes.begin(), frametimes.end());
		auto min = *std::min_element(frametimes.begin(), frametimes.end());

		ImGui::SetNextWindowPos(ImVec2(-10.f, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(_engine->WindowSize().x() + 20.f, 140.f), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.4f);
		ImGui::Begin("Framerate", nullptr, flags);

		ImGui::SetNextItemWidth(_engine->WindowSize().x());
		ImGui::PlotLines("Frametimes", frametimes.data(), frametimes.size(), 0, nullptr, 0.01f, 0.03f, ImVec2(0, 100.f));

		ImGui::End();

		ImGui::ShowDemoWindow();
	}
};

EnginePtr EngineInit();

int main()
{
	try {
		auto engine = EngineInit();
		engine->Run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}

EnginePtr EngineInit()
{
	auto desc = EngineDesc::DefaultDescriptor();
	desc.title = L"guiWindow Sample";
	auto engine = Engine::Create(&desc);

	auto scene = Scene::Create();
	engine->SetScene(scene);

	auto object = std::make_shared<SimpleWindowObject>();
	object->Init(engine);
	scene->AddObject("guiWindowObject", object);

	auto camera = ThirdPersonCamera::Create();
	scene->SetCamera(camera);

	return engine;
}
