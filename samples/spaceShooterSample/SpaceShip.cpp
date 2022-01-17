#include "SpaceShip.hpp"

#include <characterkinematic/PxBoxController.h>
#include <PxRigidDynamic.h>
#include "implementation/engine/components/RigidDynamicComponent.hpp"

void SpaceShipHitReportCallback::onShapeHit(const physx::PxControllerShapeHit& hit)
{
    //((physx::PxRigidDynamic*)hit.actor)->
    printf_s("COLLISION: %lld!\n", m_hitIndex++);
}

void SpaceShip::Init()
{
    m_camera = std::make_shared<orbit::ThirdPersonCamera>();
    m_kCom = AddComponent<orbit::KeyboardComponent>("keyboard");
    m_mCom = AddComponent<orbit::MouseComponent>("mouse");
    m_batch  = AddComponent<orbit::StaticBatchComponent>("batch" , ENGINE->RMGetIdFromName("SpaceShip"));

    m_player = m_batch->AddTransform(std::make_shared<orbit::Transform>());
    m_camera->SetTarget(m_player);
	m_camera->SetDistance(6.5f);
    m_camera->GetTransform()->Translate({ 0.1f, 0.1f, 0.1f });
    m_camera->SetProjectionHook([]() {
        const auto size = ENGINE->Get()->Window()->GetDimensions();
        const auto aspect = static_cast<float>(size.x()) / size.y();
        const auto nearZ = .2f;
        const auto farZ = 250.f;
        const auto vFOV = 65 * orbit::Math<float>::PI / 180.f;
        return orbit::Math<float>::Perspective(vFOV, aspect, nearZ, farZ);
    });

    m_hitCallback.player = this;

    physx::PxBoxControllerDesc desc;
    desc.halfHeight = .25f;
    desc.halfSideExtent = 1.8f;
    desc.halfForwardExtent = .9f;
    desc.stepOffset = .15f;
    desc.material = ENGINE->GetPhysics()->createMaterial(10.f, 10.f, 0.15f);
    desc.maxJumpHeight = 1.5f;
    desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING;
    desc.slopeLimit = .01f;
    desc.position = { 0.f, 0.f, 0.f };
    desc.reportCallback = &m_hitCallback;

    m_controller = std::unique_ptr<physx::PxController, orbit::PxDelete<physx::PxController>>(ENGINE->GetControllerManager()->createController(desc));
    m_controller->setUpDirection(physx::PxVec3{ 0.f, 1.f, 0.f });
}

void SpaceShip::Update(const orbit::Time& dt)
{
	orbit::GameObject::Update(dt);

	auto speed = m_speed * 0.5f;
    //auto speed = 0.f;

    m_horizontalSteeringForce *= m_handling * 0.8f;
    m_verticalSteeringForce *= m_handling * 0.8f;

    if (m_kCom->keydown(orbit::KeyCode::KEY_W))
    {
        m_verticalSteeringForce += 0.2f;
    }
    else if (m_kCom->keydown(orbit::KeyCode::KEY_S))
    {
        m_verticalSteeringForce -= 0.2f;
    }
    if (m_kCom->keydown(orbit::KeyCode::KEY_A))
    {
        m_horizontalSteeringForce -= 0.2f;
    }
    else if (m_kCom->keydown(orbit::KeyCode::KEY_D))
    {
        m_horizontalSteeringForce += 0.2f;
    }

    if (m_kCom->keydown(orbit::KeyCode::KEY_SPACE))
        speed *= 2.f;

    m_horizontalSteeringForce = std::clamp(m_horizontalSteeringForce, -m_maxSteeringForce, m_maxSteeringForce);
    m_verticalSteeringForce = std::clamp(m_verticalSteeringForce, -m_maxSteeringForce, m_maxSteeringForce);

    //m_player->Rotate(m_horizontalSteeringForce * dt.asSeconds(), m_player->LocalUp());
    m_player->Rotate(m_verticalSteeringForce   * 0.05f, m_player->LocalRight());
    m_player->Rotate(-m_horizontalSteeringForce * 0.1f, m_player->LocalForward());
    m_player->Rotate(m_horizontalSteeringForce * 0.05f, m_player->LocalUp());

    auto move = m_player->LocalForward();

    m_controller->setUpDirection(orbit::Math<float>::EigenToPx3(m_player->LocalUp()));
	m_controller->move(orbit::Math<float>::EigenToPx3(move * speed), 0.1f, dt.asSeconds(), physx::PxControllerFilters{});
	m_player->SetTranslation(orbit::Math<float>::PxToEigen(m_controller->getPosition()));

    auto pos = m_player->GetCombinedTranslation();
    Eigen::Vector3f leftJetPos  = pos + m_player->TransformVector({ 0.29f, -3.07f, 0.35f });
    Eigen::Vector3f rightJetPos = pos + m_player->TransformVector({ -.29f, -3.07f, 0.35f });

    m_leftJet0->_position  = { leftJetPos.x(), leftJetPos.y(), leftJetPos.z(), 1.f };
    m_rightJet0->_position = { rightJetPos.x(), rightJetPos.y(), rightJetPos.z(), 1.f };

	if (m_mCom->buttonDown(orbit::MouseComponent::MouseButton::Left) && m_mCom->mouseMoved())
	{
		m_camera->Tilt(m_mCom->mousePositionDelta().y() * dt.asSeconds() * 0.2f);
	}

	m_batch->UpdateBuffer();
}

bool SpaceShip::LoadImpl(std::ifstream*)
{
    auto scene = ENGINE->GetCurrentScene();

    m_leftJet0 = scene->AddLight(orbit::Light::CreatePointLight(
        Eigen::Vector4f{ 20.f, 10.f, 10.f, 1.f },
        Eigen::Vector4f{ 0.27f, -3.15f, .35f, 0.f },
        .05f,
        .35f
    ));
    m_rightJet0 = scene->AddLight(orbit::Light::CreatePointLight(
        Eigen::Vector4f{ 20.f, 10.f, 10.f, 1.f },
        Eigen::Vector4f{ -0.27f, -3.1f, .35f, 0.f },
        .05f,
        .35f
    ));
    return true;
}
