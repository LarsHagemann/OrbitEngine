#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "Orbit.lib")

#include <iostream>
#include <fstream>
#include <type_traits>
#include <random>

#include "Engine/Engine.hpp"
#include "Engine/Component/KeyboardComponent.hpp"
#include "Engine/Component/MouseComponent.hpp"
#include "Engine/Component/BatchComponent.hpp"
#include "Engine/Component/StateComponent.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Rendering/Cameras/ThirdPersonCamera.hpp"
#include "Engine/Object.hpp"
#include "Engine/Scene.hpp"
#include "Engine/DebugObject.hpp"
#include "Engine/Misc/Random.hpp"
#include "Engine/Misc/Noise/OpenSimplexNoise.hpp"

#include "imgui.h"

using namespace orbit;

// This component implements a torch that is
// attached to a certain transform (moving the transform
// will move the torch)
class TorchComponent : public EventDriven, public Component
{
private:
	LightPtr _light;
	TransformPtr _transform;
	ScenePtr _scene;
	Clock _lifeClock;
	float _randomizer;
	float _baseStrength;
	float _timeMultiplier;
	OpenSimplexNoise _noise;
public:
	TorchComponent(ObjectPtr object, ScenePtr scene, TransformPtr target) :
		Component(object),
		_light(Light::CreatePointLight({ 800.f, 500.f, 500.f, 1.f }, Vector4f::Zero(), 0.1f, 0.2f)),
		_randomizer(Random::UniformNumber(0.f, 2.f)),
		_baseStrength(1.5f),
		_timeMultiplier(0.01f),
		_noise(Random::UniformNumber(0ll, std::numeric_limits<int64_t>::max())),
		_transform(target)
	{
		scene->AddLight(_light);
	}
	static std::shared_ptr<TorchComponent> create(ObjectPtr object, ScenePtr scene, TransformPtr target)
	{
		return std::make_shared<TorchComponent>(object, scene, target);
	}
	float calculateStrength()
	{
		return static_cast<float>(
			(_noise.Evaluate(static_cast<double>(
				_lifeClock.GetElapsedTime().asMilliseconds() * _timeMultiplier), _randomizer) + 1.) / 2.);
	}
	virtual void Update(Time dt) override
	{
		// Color = 237, 196, 14
		auto normStrength = calculateStrength();
		auto strength = (normStrength + 3.f) / 4.f * _baseStrength;
		_light->_color = Vector4f{ 237 * strength, 196 * strength, 14 * strength, 1.f };
		auto p = _transform->GetCombinedTranslation();
		_light->_position = { p.x(), p.y(), p.z(), 0.f };
	}
};

// The Player Object
class OrbWindowObject : public Object
{
protected:
	std::shared_ptr<KeyboardComponent> _kHandler;
	std::shared_ptr<MouseComponent> _mHandler;
	std::shared_ptr<ThirdPersonCamera> _camera;
	bool _debugCursor = false;
	TransformPtr _player;
	float _speed;
	float _mouseSensitivity;
	ScenePtr _scene;
	std::shared_ptr<TorchComponent> _torch;
	std::shared_ptr<StateComponent<>> _stateManager;
public:
	void SetScene(ScenePtr scene) { _scene = scene; }
	virtual void Init() override
	{
		Object::Init();
		_speed = 0.25f;
		_mouseSensitivity = 0.001f;
		_kHandler = AddComponent<KeyboardComponent>("keyboard_controller");
		_mHandler = AddComponent<MouseComponent>("mouse_controller");
		_player = GetStatic<orbit::BatchComponent>("Cube")->AddTransform();
		_player->SetScaling(0.3f);
		_player->SetTranslation({ 0.f, 1.f, 0.f });
		_camera = ThirdPersonCamera::Create();
		_camera->SetTarget(_player);
		_torch = AddComponent<TorchComponent>("player_torch", _scene, _player);
		_stateManager = AddComponent<StateComponent<>>("state", _kHandler, _mHandler);
		_stateManager->AddState("default");
		_stateManager->AddState("forward");
		_stateManager->AddState("backward");
		_stateManager->AddState("single_click");
		_stateManager->AddState("double_click");

		_stateManager->Transition_OnKeyDown(0, 1, DIK_W);
		_stateManager->Transition_OnKeyUp(1, 0, DIK_W);
		
		_stateManager->Transition_OnKeyDown(0, 2, DIK_S);
		_stateManager->Transition_OnKeyUp(2, 0, DIK_S);

		_stateManager->Transition_OnMouseKeyDown(0, 3, MouseComponent::MouseButton::Left);
		_stateManager->Transition_OnMouseKeyDown(3, 4, MouseComponent::MouseButton::Left);
		_stateManager->Transition_OnTimeElapsed(4, 0, 1000);
		_stateManager->Transition_OnTimeElapsed(3, 0, 500);

		_stateManager->RegisterOnStateEnterCallback(4, [](size_t, std::string_view) { ORBIT_INFO("Super power function used!"); });
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
			_kHandler->keydown(DIK_W) ? -1.f : (_kHandler->keydown(DIK_S) ? 1.f : 0.f),
			_kHandler->keydown(DIK_SPACE) ? 1.f : (_kHandler->keydown(DIK_LSHIFT) ? -1.f : 0.f)
		});

		auto tilt = static_cast<float>(_mHandler->mousePositionDelta().y());
		auto pan = static_cast<float>(-_mHandler->mousePositionDelta().x());
		if (_debugCursor)
		{
			tilt = 0.f;
			pan = 0.f;
		}

		_player->Rotate(pan * _mouseSensitivity, Eigen::Vector3f{ 0.f, 0.f, 1.f });
		_camera->Tilt(tilt * _mouseSensitivity);

		_player->Translate(movement * dt.asSeconds() * _speed);
		Engine::Get()->GetDebugObject()->ShowText("State: %d / %s", _stateManager->GetCurrentStateId(), _stateManager->GetCurrentStateName().data());
	}
};

// The Environment object
class SceneObject : public Object
{
public:
	virtual void Init() override
	{
		Object::Init();

		GetStatic<BatchComponent>("Plane")->AddTransform()->SetScaling(0.1f);
		GetStatic<BatchComponent>("Cone")->AddTransform()->SetScaling(0.1f);
		GetStatic<BatchComponent>("Cylinder")->AddTransform()->SetScaling(0.1f);
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
	catch (Exception& e)
	{
		std::cout << e.what() << '\n';
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
	object->SetScene(scene);
	object->Init();
	scene->SetCamera(object->GetCamera());
	scene->SetAmbientLighting({ 0.15f, 0.15f, 0.15f, 1.f });

	auto sceneObject = std::make_shared<SceneObject>();
	scene->AddObject("sceneObject", sceneObject);
	scene->LoadOrb("assets/scene.orb", sceneObject);
	sceneObject->Init();

	scene->AddLight(Light::CreateDirectionalLight(Vector4f{ 15.f, 15.f, 15.f, 1.f }, Vector4f(0.f, -1.f, 0.f, 0.f)));

	return engine;
}
