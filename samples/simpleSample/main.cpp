#include "implementation/misc/Logger.hpp"
#include "interfaces/misc/Loggable.hpp"
#include "implementation/backends/impl/WindowImpl.hpp"
#include "implementation/engine/components/StateComponent.hpp"
#include "implementation/engine/components/KeyboardComponent.hpp"
#include "implementation/engine/components/MouseComponent.hpp"
#include "implementation/engine/components/StaticBatchComponent.hpp"
#include "implementation/engine/components/RigidStaticComponent.hpp"
#include "implementation/engine/components/RigidDynamicComponent.hpp"
#include "implementation/rendering/ThirdPersonCamera.hpp"
#include "implementation/backends/Platform.hpp"
#include "implementation/engine/Engine.hpp"

#undef new

#include <characterkinematic/PxBoxController.h>
#include <characterkinematic/PxController.h>

#include <imgui.h>

class MyComponent : public orbit::Updatable
{
public:
	MyComponent(orbit::GameObject* object) :
		orbit::Updatable(object)
	{
	}
	virtual void Update(const orbit::Time& dt) override
	{
	}
};

class MyObject : public orbit::GameObject
{
private:
	std::shared_ptr<orbit::KeyboardComponent> m_kCom;
	std::shared_ptr<orbit::MouseComponent> m_mCom;
	std::shared_ptr<orbit::ThirdPersonCamera> m_camera;
	std::shared_ptr<orbit::StaticBatchComponent> m_batch;
	std::unique_ptr<physx::PxController, orbit::PxDelete<physx::PxController>> m_controller;
	bool m_isFullscreen = false;
	float m_speed = 1.f;
	orbit::TransformPtr m_player;
public:
	bool LoadImpl(std::ifstream*) override { 
		return true;
	}
	void UnloadImpl() override {}
	virtual void Init() override
	{
		AddComponent<MyComponent>("my_component");
		m_camera = std::make_shared<orbit::ThirdPersonCamera>();
		m_kCom = AddComponent<orbit::KeyboardComponent>("keyboard");
		m_mCom = AddComponent<orbit::MouseComponent>("mouse");
		auto s = AddComponent<orbit::StateComponent<>>("state", m_kCom, m_mCom);
		m_batch  = AddComponent<orbit::StaticBatchComponent>("batch" , ENGINE->RMGetIdFromName("Player"));
		auto bc2 = AddComponent<orbit::StaticBatchComponent>("batch2", ENGINE->RMGetIdFromName("Sphere"));
		auto bc3 = AddComponent<orbit::StaticBatchComponent>("batch3", ENGINE->RMGetIdFromName("Plane"));
		auto body0 = AddComponent<orbit::RigidStaticComponent>("body0", ENGINE->RMGetIdFromName("Sphere"));
		body0->CookBody(orbit::MaterialProperties::DefaultMaterial(), { 1.f, 1.f, 1.f }, 0);

		bc3->AddTransform(std::make_shared<orbit::Transform>());

		m_player = m_batch->AddTransform(std::make_shared<orbit::Transform>());
		
		m_camera->SetTarget(m_player);
		m_camera->GetTransform()->Translate({ 0.1f, 0.1f, 0.1f });
		m_camera->SetProjectionHook([]() {
			const auto size = ENGINE->Get()->Window()->GetDimensions();
			const auto aspect = static_cast<float>(size.x()) / size.y();
			const auto nearZ = 2.f;
			const auto farZ = 300.f;
			const auto vFOV = 65 * orbit::Math<float>::PI / 180.f;
			return orbit::Math<float>::Perspective(vFOV, aspect, nearZ, farZ);
		});

		physx::PxBoxControllerDesc desc;
		desc.halfHeight = 1.f;
		desc.halfSideExtent = 1.f;
		desc.halfForwardExtent = 1.f;
		desc.material = ENGINE->GetPhysics()->createMaterial(10.f, 10.f, 0.15f);
		desc.maxJumpHeight = 1.5f;
		desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
		desc.slopeLimit = .64f;
		desc.position = { 5.f, 0.f, 0.f };
		//desc.reportCallback = 

		m_controller = std::unique_ptr<physx::PxController, orbit::PxDelete<physx::PxController>>(ENGINE->GetControllerManager()->createController(desc));
		m_controller->setUpDirection(physx::PxVec3{ 0.f, 1.f, 0.f });

		auto rd = std::random_device{};
		auto re = std::mt19937(rd());

		std::uniform_real_distribution<float> dist(-100.f, 100.f);

		for (auto i = 0u; i < 1000; ++i) 
		{
			auto t = bc2->AddTransform(std::make_shared<orbit::Transform>());
			t->Translate({ dist(re), dist(re), 1.f });

			body0->AddActor(t);
		}
	}

	virtual void Update(const orbit::Time& dt) override
	{
		orbit::GameObject::Update(dt);
		if (m_kCom->keydownThisFrame(orbit::KeyCode::KEY_F11))
		{
			m_isFullscreen = !m_isFullscreen;
			ENGINE->Window()->ToggleFullscreen(m_isFullscreen);
		}
		if (m_kCom->keydownThisFrame(orbit::KeyCode::KEY_ESCAPE))
			ENGINE->Window()->Close();

		auto speed = m_speed;
		if (m_kCom->keydown(orbit::KeyCode::KEY_LSHIFT))
			speed *= 0.25f;

		Eigen::Vector3f move = Eigen::Vector3f::Zero();

		if (m_kCom->keydown(orbit::KeyCode::KEY_W))
			move.y() = 1.f;
		if (m_kCom->keydown(orbit::KeyCode::KEY_S))
			move.y() = -1.f;
		if (m_kCom->keydown(orbit::KeyCode::KEY_A))
			move.x() = 1.f;
		if (m_kCom->keydown(orbit::KeyCode::KEY_D))
			move.x() = -1.f;

		move = m_player->TransformVector(move);
		move.normalize();
		
		m_controller->move(orbit::Math<float>::EigenToPx3(move * speed), 0.1f, dt.asSeconds(), physx::PxControllerFilters{});
		m_player->SetTranslation(orbit::Math<float>::PxToEigen(m_controller->getFootPosition()));

		if (m_mCom->buttonDown(orbit::MouseComponent::MouseButton::Left) && m_mCom->mouseMoved())
		{
			m_player->Rotate(m_mCom->mousePositionDelta().x() * dt.asSeconds(), m_player->LocalUp());
			m_camera->Tilt(m_mCom->mousePositionDelta().y() * dt.asSeconds() * 0.2f);
		}

		m_batch->UpdateBuffer();
	}
	std::shared_ptr<orbit::ThirdPersonCamera> GetCamera() const { return m_camera; }
};

class MyScene : public orbit::AsyncScene
{
public:
	MyScene()
	{
		auto object = std::make_shared<MyObject>();
		AddObject("my_object", object);
		SetCamera(object->GetCamera());
	}
	virtual void Update(const orbit::Time& dt) override
	{
		AsyncScene::Update(dt);
	}
};

int main()
{
	{
		orbit::EngineInitDesc desc;
		desc.msaa = 8;
		desc.numThreads = 1;

		auto window = orbit::Window::Create({ 1080, 720 }, L"Simple Sample");
		ENGINE->Init(window, desc);
		//ENGINE->EnableVSync(true);
		ENGINE->RMParseFile("torus.orb");
		auto scene = std::make_shared<MyScene>();
		ENGINE->RegisterScene("my_scene", scene);
		ENGINE->EnterScene("my_scene");
		scene->AddLight(orbit::Light::CreatePointLight(
			{ .9f, .5f, .4f, 1.f },
			orbit::Vector4f(0.f, 0.f, 3.f, 1.f),
			30.f,
			200.f)
		);
		ENGINE->Run();
		ENGINE->Shutdown();
	}

	shutdown_monitoring();
	return 0;
}