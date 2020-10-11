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
#include "ImGui/imgui.h"

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
	desc.title = L"simpleWindow Sample";
	auto engine = Engine::Create(&desc);

	auto scene = Scene::Create();
	engine->SetScene(scene);
	//engine->SetFramerateLimit(60);

	auto object = std::make_shared<SimpleWindowObject>();
	object->Init(engine);
	scene->AddObject("simpleWindowObject", object);

	auto camera = ThirdPersonCamera::Create();
	scene->SetCamera(camera);

	return engine;
}
