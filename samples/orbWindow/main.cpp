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
#include "Engine/Component/RigidStaticComponent.hpp"
#include "Engine/Component/EntityControllerComponent.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Misc/Math.hpp"
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

#include "PxMaterial.h"

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
	std::shared_ptr<StateComponent<>> _boostState;
	PxController* _controller;
	Vector3f m_movement;
	float m_colliderHeight = .17f;

	struct PlayerCallback : public PxUserControllerHitReport
	{
		virtual void onShapeHit(const PxControllerShapeHit& hit) override
		{
		}
		virtual void onControllerHit(const PxControllersHit& hit) override
		{
		}
		virtual void onObstacleHit(const PxControllerObstacleHit& hit) override
		{
		}
	} m_callback;
public:
	void SetScene(ScenePtr scene) { _scene = scene; }
	virtual void Init() override
	{
		Object::Init();
		_speed = 5.f;
		_mouseSensitivity = 0.001f;
		_kHandler = AddComponent<KeyboardComponent>("keyboard_controller");
		_mHandler = AddComponent<MouseComponent>("mouse_controller");
		_player = GetStatic<orbit::BatchComponent>("Cube")->AddTransform();
		_player->SetScaling(0.3f);
		_player->SetTranslation({ 0.f, 0.f, .125f });
		_camera = ThirdPersonCamera::Create();
		_camera->SetTarget(_player);
		_torch = AddComponent<TorchComponent>("player_torch", _scene, _player);
		_boostState = AddComponent<StateComponent<>>("boost", _kHandler, _mHandler);


		PxCapsuleControllerDesc desc;
		desc.height = m_colliderHeight;
		desc.radius = m_colliderHeight * 0.4f;
		desc.material = Engine::Get()->GetPhysics()->createMaterial(10.f, 10.f, 0.15f);
		desc.maxJumpHeight = .4f;
		desc.position = PxExtendedVec3(0.f, 0.f, 1.f);
		desc.reportCallback = &m_callback;
		desc.slopeLimit = 0.0f;
		desc.stepOffset = m_colliderHeight * 0.33f;
		desc.upDirection = PxVec3{ 0.f, 0.f, 1.f };
		desc.volumeGrowth = 1.6f;
		desc.contactOffset = 0.01f;

		_controller = AddComponent<PlayerControllerComponent>("player_controller", std::ref(desc))->GetController();

		_boostState->AddState("default");
		_boostState->AddState("prepare_boost_w");
		_boostState->AddState("prepare_boost_a");
		_boostState->AddState("prepare_boost_s");
		_boostState->AddState("prepare_boost_d");
		_boostState->AddState("boost");
		_boostState->AddState("cooldown");

		_boostState->Transition_OnKeyDown("default", "prepare_boost_w", DIK_W);
		_boostState->Transition_OnTimeElapsed("prepare_boost_w", "default", 200);
		_boostState->Transition_OnKeyDown("prepare_boost_w", "boost", DIK_W);

		_boostState->Transition_OnKeyDown("default", "prepare_boost_a", DIK_A);
		_boostState->Transition_OnTimeElapsed("prepare_boost_a", "default", 200);
		_boostState->Transition_OnKeyDown("prepare_boost_a", "boost", DIK_A);

		_boostState->Transition_OnKeyDown("default", "prepare_boost_s", DIK_S);
		_boostState->Transition_OnTimeElapsed("prepare_boost_s", "default", 200);
		_boostState->Transition_OnKeyDown("prepare_boost_s", "boost", DIK_S);

		_boostState->Transition_OnKeyDown("default", "prepare_boost_d", DIK_D);
		_boostState->Transition_OnTimeElapsed("prepare_boost_d", "default", 200);
		_boostState->Transition_OnKeyDown("prepare_boost_d", "boost", DIK_D);

		_boostState->Transition_OnTimeElapsed("boost", "cooldown", 120);
		_boostState->Transition_OnTimeElapsed("cooldown", "default", 2000);

		_boostState->RegisterOnStateEnterCallback("boost", [&](size_t, std::string_view) {
			_speed = 5.f;
			}
		);
		_boostState->RegisterOnStateEnterCallback("cooldown", [&](size_t, std::string_view) {
			_speed = 0.25f;
			}
		);
	}

	std::shared_ptr<ThirdPersonCamera> GetCamera() const { return _camera; }

	virtual void Update(Time dt) override
	{
		if (_kHandler->keydownThisFrame(DIK_ESCAPE))
			Engine::Get()->GetWindow()->Close();
		if (_kHandler->keydownThisFrame(DIK_F11))
			Engine::Get()->GetWindow()->SetFullscreen(!Engine::Get()->GetWindow()->IsFullscreen());
		if (_kHandler->keydownThisFrame(DIK_F4))
			_debugCursor = !_debugCursor;

		auto localUp = _player->LocalUp();
		_player->SetTranslation(Math<float>::PxToEigen(_controller->getPosition()) + Vector3f::UnitZ() * (m_colliderHeight * 2.3f));
		_controller->setUpDirection(Math<>::EigenToPx3(localUp));

		auto movement = _player->TransformVector({
			_kHandler->keydown(DIK_A) ? 1.f : (_kHandler->keydown(DIK_D) ? -1.f : 0.f),
			_kHandler->keydown(DIK_W) ? 1.f : (_kHandler->keydown(DIK_S) ? -1.f : 0.f),
			_kHandler->keydown(DIK_SPACE) ? 1.f : (_kHandler->keydown(DIK_LSHIFT) ? -1.f : 0.f)
		});
		//movement += localUp * -4.f; // gravity

		m_movement = Math<>::Lerp(m_movement * (1.f - dt.asSeconds()), movement * 15.f, 1.f / 300.f);

		static int spaceCounter = 0;

		if (_kHandler->keydownThisFrame(DIK_SPACE))
		{
			//m_movement.z() += 18.f;
			++spaceCounter;
		}

		Engine::Get()->GetDebugObject()->ShowText("Space down: %d", spaceCounter);

		auto tilt = static_cast<float>(-_mHandler->mousePositionDelta().y());
		auto pan = static_cast<float>(_mHandler->mousePositionDelta().x());
		if (_debugCursor)
		{
			tilt = 0.f;
			pan = 0.f;
		}

		_player->Rotate(pan * _mouseSensitivity, Eigen::Vector3f{ 0.f, 0.f, 1.f });
		_camera->Tilt(tilt * _mouseSensitivity);

		//
		//_player->Translate(movement * dt.asSeconds() * _speed);
		Engine::Get()->GetDebugObject()->ShowText("State: %d / %s", _boostState->GetCurrentStateId(), _boostState->GetCurrentStateName().data());
		Engine::Get()->GetDebugObject()->ShowText("Speed: %f", _speed);
	}

	virtual void PhysicsUpdate(size_t millis) override
	{
		_controller->move(Math<>::EigenToPx3(m_movement) * 0.25f, 0.1f, 1000.f / millis, PxControllerFilters());
	}

};

#include "Engine/Misc/Noise/OpenSimplexNoise.hpp"

// The Environment object
class SceneObject : public Object
{
public:
	virtual void Init() override
	{
		Object::Init();

		//GetStatic<BatchComponent>("Plane")->AddTransform()->SetScaling(0.1f);
		GetStatic<BatchComponent>("Cylinder.001")->AddTransform()->SetScaling(0.1f);
		//GetStatic<BatchComponent>("Cylinder")->AddTransform()->SetScaling(0.1f);

		auto noise = OpenSimplexNoise(0);

		auto groundMesh = Mesh::CreatePlaneMesh(5, "TreeMaterial", [&](Vector3f& pos) { pos *= 100.f; pos.z() = noise.Evaluate(pos.x() / 10.f, pos.y() / 15.f) * 15.f; });
		AddComponent<BatchComponent>("ground", groundMesh)->AddTransform();
		
		auto body = AddComponent<RigidStaticComponent>("rigid_place_body", GetStatic<BatchComponent>("ground")->GetMesh());
		body->CookBody(Vector3f::Zero(), Quaternionf::Identity(), MaterialProperties{ .7f, .7f, .1f }, Vector3f{ 1.f, 1.f, 1.f });
	}
};

EnginePtr EngineInit();

#include "Engine/PhysX/PxOrbitErrorCallback.hpp"

#include <extensions/PxDefaultAllocator.h>
#include <pvd/PxPvdTransport.h>
#include <PxPhysicsVersion.h>
#include <extensions/PxExtensionsAPI.h>

int main()
{
#ifndef _DEBUG
	std::ofstream log("log.txt");
	auto cachedCout = std::cout.rdbuf();
	auto cachedClog = std::clog.rdbuf();
	auto cachedCerr = std::cerr.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	std::clog.rdbuf(log.rdbuf());
	std::cerr.rdbuf(log.rdbuf());
#endif
	try {
		Logger::sInfoLevel = Logger::sInfoLevel_Debug;
		auto engine = EngineInit();

		engine->Restart();
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
#ifndef _DEBUG
	log.flush();
	std::cout.rdbuf(cachedCout);
	std::clog.rdbuf(cachedClog);
	std::cerr.rdbuf(cachedCerr);
	log.close();
#endif
}

EnginePtr EngineInit()
{
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

	scene->AddLight(Light::CreateDirectionalLight(Vector4f{ 15.f, 15.f, 15.f, 1.f }, Vector4f(0.f, -1.f, 0.f, 1.f)));

	return engine;
}
