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

using namespace orbit;

class SimpleWindowObject : public Object
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

	}
};

std::shared_ptr<Engine> EngineInit();

int main()
{
	try {
		auto engine = EngineInit();
		engine->Run();
	}
	catch (Exception& e)
	{
		std::cout << e.what() << '\n';
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << '\n';
	}
}

std::shared_ptr<Engine> EngineInit()
{
	auto window = Window::Create({ 1080, 600 }, L"simpleWindow");
	auto desc = InitDesc::GetDefaultDesc();
	auto engine = Engine::Init(window, &desc);

	auto scene = Scene::Create();
	engine->SetScene(scene);
	//engine->SetFramerateLimit(60);
	
	auto object = std::make_shared<SimpleWindowObject>();
	object->Init();
	scene->AddObject("simpleWindowObject", object);
	
	//auto camera = ThirdPersonCamera::Create();
	//scene->SetCamera(camera);

	return engine;
}
