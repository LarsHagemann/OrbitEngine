#pragma once
#include "implementation/rendering/ThirdPersonCamera.hpp"
#include "implementation/engine/components/KeyboardComponent.hpp"
#include "implementation/engine/components/MouseComponent.hpp"
#include "implementation/engine/components/StaticBatchComponent.hpp"
#include "implementation/engine/GameObject.hpp"

#include <characterkinematic/PxController.h>

class SpaceShip;

struct SpaceShipHitReportCallback : public physx::PxUserControllerHitReport
{
	SpaceShip* player;
	uint64_t m_hitIndex = 0;

	virtual void onShapeHit(const physx::PxControllerShapeHit& hit) override;
	virtual void onControllerHit(const physx::PxControllersHit& hit) override
	{
	}
	virtual void onObstacleHit(const physx::PxControllerObstacleHit& hit) override
	{
	}
};

class SpaceShip : public orbit::GameObject
{
private:
    std::shared_ptr<orbit::KeyboardComponent> m_kCom;
	std::shared_ptr<orbit::MouseComponent> m_mCom;
	std::shared_ptr<orbit::ThirdPersonCamera> m_camera;
	std::shared_ptr<orbit::StaticBatchComponent> m_batch;
	std::unique_ptr<physx::PxController, orbit::PxDelete<physx::PxController>> m_controller;
	physx::PxShape* m_controllerShape;

	float m_speed = 1.7f;
	float m_horizontalSteeringForce = 0.f;
	float m_verticalSteeringForce = 0.f;
	float m_maxSteeringForce = orbit::Math<float>::PI * 0.5f;
	float m_handling = .8f;
	float m_rotationY;
	float m_rotationX;
	float m_rotationZ;

	orbit::Light* m_leftJet0;
	orbit::Light* m_rightJet0;
	orbit::Light* m_leftJet1;
	orbit::Light* m_rightJet1;
	orbit::TransformPtr m_player;

	SpaceShipHitReportCallback m_hitCallback;
public:
	bool LoadImpl(std::ifstream*) override;
	void UnloadImpl() override {}

    virtual void Init() override;
    virtual void Update(const orbit::Time& dt) override;
    std::shared_ptr<orbit::ThirdPersonCamera> GetCamera() const { return m_camera; }
};
