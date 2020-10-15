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
#include "batch_component.hpp"
#include "exception.hpp"

using namespace orbit;

class OrbWindowObject : public Object
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
	}
};

EnginePtr EngineInit();

int main()
{
	try {
		OrbitLogger::sInfoLevel = 10;
		auto engine = EngineInit();
		engine->Run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
	catch (const char* e)
	{
		std::cout << e << '\n';
	}
	catch (...)
	{
		// Undetectable exception
	}
}

EnginePtr EngineInit()
{
	auto desc = EngineDesc::DefaultDescriptor();
	desc.title = L"orbWindow Sample";
	desc.clearColor = Vector4f { 0.83f, 0.686f, 0.216f, 1.f };
	auto engine = Engine::Create(&desc, L"orbWindow");
	auto scene = Scene::Create(engine);
	engine->SetScene(scene);

	auto object = std::make_shared<OrbWindowObject>();
	object->Init(engine);
	scene->LoadOrb("scene.orb", object);
	scene->AddObject("orbWindowObject", object);

	object->GetStatic<orbit::BatchComponent>("Monkey")->AddTransform();
	object->GetStatic<orbit::BatchComponent>("Monkey")->AddTransform();

	auto camera = ThirdPersonCamera::Create();
	scene->SetCamera(camera);

	return engine;
}
