#pragma once
#include "implementation/engine/components/StaticBatchComponent.hpp"
#include "implementation/engine/components/RigidDynamicComponent.hpp"
#include "implementation/engine/GameObject.hpp"

#include <characterkinematic/PxController.h>

struct Asteroid
{
	orbit::TransformPtr transform;
	Eigen::Vector3f velocity;
};

class AsteroidController : public orbit::GameObject
{
private:
	std::shared_ptr<orbit::StaticBatchComponent> m_batch;
    std::shared_ptr<orbit::RigidDynamicComponent> m_body;
	float m_minSpeed = 0.7f;
	float m_maxSpeed = 4.5f;
	std::vector<Asteroid> m_asteroids;
public:
    bool LoadImpl(std::ifstream*) override { 
		return true;
	}
	void UnloadImpl() override {}

    virtual void Init() override;
    virtual void Update(const orbit::Time& dt) override;
};
