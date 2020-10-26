#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "Orbit.lib")

#include <iostream>
#include <fstream>
#include <random>

#include "Engine/Engine.hpp"
#include "Engine/Component/KeyboardComponent.hpp"
#include "Engine/Component/MouseComponent.hpp"
#include "Engine/Component/BatchComponent.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Rendering/Cameras/ThirdPersonCamera.hpp"
#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"

#include "imgui.h"

using namespace orbit;

class OrbWindowObject : public Object
{
protected:
	std::shared_ptr<KeyboardComponent> _kHandler;
	std::shared_ptr<MouseComponent> _mHandler;
	std::shared_ptr<ThirdPersonCamera> _camera;
	bool _debugCursor = false;
	TransformPtr _player;
public:
	virtual void Init() override
	{
		Object::Init();
		_kHandler = AddComponent<KeyboardComponent>("keyboard_controller");
		_mHandler = AddComponent<MouseComponent>("mouse_controller");
		_player = GetStatic<orbit::BatchComponent>("Sphere")->AddTransform();
		_player->SetScaling(0.3f);
		_camera = ThirdPersonCamera::Create();
		_camera->SetTarget(_player);
	}

	std::shared_ptr<ThirdPersonCamera> GetCamera() const { return _camera; }

	virtual void Update(Time dt) override
	{
		if (_kHandler->keydownThisFrame(DIK_ESCAPE))
			Engine::Get()->GetWindow()->Close();
		if (_kHandler->keydownThisFrame(DIK_F11))
			Engine::Get()->GetWindow()->SetFullscreen(Engine::Get()->GetWindow()->IsFullscreen());
		if (_kHandler->keydownThisFrame(DIK_F4))
			_debugCursor = !_debugCursor;

		Vector3f movement = _player->TransformVector({
			_kHandler->keydown(DIK_A) ? 1.f : (_kHandler->keydown(DIK_D) ? -1.f : 0.f),
			_kHandler->keydown(DIK_W) ? 1.f : (_kHandler->keydown(DIK_S) ? -1.f : 0.f),
			_kHandler->keydown(DIK_SPACE) ? 1.f : (_kHandler->keydown(DIK_LSHIFT) ? -1.f : 0.f)
		});

		auto tilt = static_cast<float>(-_mHandler->mousePositionDelta().y());
		auto pan = static_cast<float>(_mHandler->mousePositionDelta().x());
		if (_debugCursor)
		{
			tilt = 0.f;
			pan = 0.f;
		}

		_player->Rotate(pan * 0.01f, Eigen::Vector3f{ 0.f, 0.f, 1.f });
		_camera->Tilt(tilt * 0.01f);

		_player->Translate(movement * .001f);
	}
};

class SceneObject : public Object
{
public:
	virtual void Init() override
	{
		Object::Init();

		std::mt19937 engine(0);
		std::uniform_real_distribution<float> dist(-.5f, .5f);
		std::uniform_real_distribution<float> angle(0.f, Math<float>::_2PI);

		for (auto i = 0u; i < 100; ++i)
		{
			auto t = GetStatic<BatchComponent>("Cube")->AddTransform();
			t->SetTranslation(Vector3f{ dist(engine), dist(engine), 0.f });
			t->SetRotation(angle(engine), Vector3f::UnitZ());
			t->SetScaling(0.1f);
		}

		//GetStatic<BatchComponent>("Plane")->AddTransform();
		GetStatic<BatchComponent>("Cube.001")->AddTransform()->SetScaling(0.1f);
		GetStatic<BatchComponent>("Cube.002")->AddTransform()->SetScaling(0.1f);
		GetStatic<BatchComponent>("Cube.003")->AddTransform()->SetScaling(0.1f);
	}
};

EnginePtr EngineInit();

int main()
{
	try {
		Logger::sInfoLevel = 10;
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
#ifndef _DEBUG
	std::ofstream log("log.txt");
	std::cout.rdbuf(log.rdbuf());
	std::clog.rdbuf(log.rdbuf());
	std::cerr.rdbuf(log.rdbuf());
#endif

	auto window = Window::Create({ 1080, 600 }, L"orbWindow sample");
	auto desc = InitDesc::GetDefaultDesc();
	desc.numThreads = 1u;
	auto engine = Engine::Init(window, &desc, L"orbWindow");
	auto scene = Scene::Create();
	engine->SetScene(scene);

	auto object = std::make_shared<OrbWindowObject>();
	scene->AddObject("orbWindowObject", object);
	scene->LoadOrb("assets/player.orb", object);
	object->Init();
	scene->SetCamera(object->GetCamera());

	auto sceneObject = std::make_shared<SceneObject>();
	scene->AddObject("sceneObject", sceneObject);
	scene->LoadOrb("assets/scene.orb", sceneObject);
	sceneObject->Init();

	return engine;
}
