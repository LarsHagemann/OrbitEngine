#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "Orbit.lib")

#include <iostream>
#include "Engine/Window.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"
#include "Engine/Component/KeyboardComponent.hpp"
#include "Engine/Rendering/Cameras/ThirdPersonCamera.hpp"

#include "imgui.h"

using namespace orbit;

class GuiWindowObject : public Object
{
protected:
	std::shared_ptr<KeyboardComponent> _kHandler;
public:
	virtual void Init() override
	{
		Object::Init();
		_kHandler = AddComponent<KeyboardComponent>("keyboard_controller");
	}

	virtual void Update(Time dt) override
	{
		if (_kHandler->keydownThisFrame(DIK_ESCAPE))
			Engine::Get()->GetWindow()->Close();
		if (_kHandler->keydownThisFrame(DIK_F11))
			Engine::Get()->GetWindow()->SetFullscreen(Engine::Get()->GetWindow()->IsFullscreen());

		ImGui::ShowDemoWindow();
	}
};

std::shared_ptr<Engine> EngineInit();

int main()
{
	//try {
		auto engine = EngineInit();
		engine->Run();
	//}
	//catch (std::exception& e)
	//{
	//	ORBIT_ERR(e.what());
	//}
}

std::shared_ptr<Engine> EngineInit()
{
	auto window = Window::Create({ 1080, 600 }, L"guiWindow");
	auto desc = InitDesc::GetDefaultDesc();
	desc.numThreads = 1u;
	auto engine = Engine::Init(window, &desc, L"guiWindow");

	auto scene = Scene::Create();
	engine->SetScene(scene);
	//engine->SetFramerateLimit(60);

	auto object = std::make_shared<GuiWindowObject>();
	object->Init();
	scene->AddObject("guiWindowObject", object);

	auto camera = ThirdPersonCamera::Create();
	scene->SetCamera(camera);

	return engine;
}
