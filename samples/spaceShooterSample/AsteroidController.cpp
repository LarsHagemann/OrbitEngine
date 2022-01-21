#include "AsteroidController.hpp"
#include <random>

void AsteroidController::Init()
{
    m_batch  = AddComponent<orbit::StaticBatchComponent>("batch", ENGINE->RMGetIdFromName("Asteroid"));
	m_body   = AddComponent<orbit::RigidDynamicComponent>("body", ENGINE->RMGetIdFromName("Asteroid"));
	m_body->CookBody(orbit::MaterialProperties::DefaultMaterial(), { 3.f, 3.f, 3.f }, 0);

	auto device = std::random_device{};
	auto engine = std::mt19937{ device() };
	auto dist = std::uniform_real_distribution<float>{ -1.f, 1.f };
	auto speed_dist = std::uniform_real_distribution<float>{ m_minSpeed, m_maxSpeed };

#ifdef _DEBUG
	constexpr auto numAsteroids = 200;
#else
	constexpr auto numAsteroids = 2000;
#endif

	for (auto i = 0u; i < numAsteroids; ++i)
	{
		auto t = m_batch->AddTransform(std::make_shared<orbit::Transform>());
		auto theta0 = orbit::Math<float>::PI * 2.f * dist(engine);
		auto theta1 = orbit::Math<float>::PI * 2.f * dist(engine);
		auto posX = dist(engine);
		auto posY = dist(engine);
		auto posZ = dist(engine);
		auto speed = speed_dist(engine);
		Asteroid asteroid;
		asteroid.transform = t;
		asteroid.velocity = 0.004f * Eigen::Vector3f{
			sin(theta0) * sin(theta1),
			cos(theta1),
			cos(theta0) * sin(theta1)
		};

		t->SetTranslation({
			400.f * posX,
			200.f * posY,
			400.f * posZ
		});
		t->SetScaling(3.f);
		m_asteroids.emplace_back(asteroid);
		auto id = m_body->AddActor(t);
		m_body->GetRigidDynamic(id)->setLinearVelocity(orbit::Math<float>::EigenToPx3(asteroid.velocity));
	}	
}

void AsteroidController::Update(const orbit::Time& dt)
{
	orbit::GameObject::Update(dt);
	//for (auto& asteroid : m_asteroids)
	//{
	//	auto movement = asteroid.velocity * dt.asSeconds();
	//	asteroid.transform->Translate(movement);
	//}

	m_batch->UpdateBuffer();
}
