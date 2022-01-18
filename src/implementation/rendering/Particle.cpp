#include "implementation/rendering/Particle.hpp"
#include "implementation/rendering/ParticleSystem.hpp"

namespace orbit
{

    Particle::Particle(const ParticleDesc& desc) :
        m_system(desc.system),
        m_transform(desc.transform),
        m_id(desc.id),
        m_lifeExpectancy(desc.lifeExpectancy),
        m_body(desc.body)
    {        
    }

    Particle::~Particle()
    {

    }

    void Particle::Update(const Time& dt)
    {
        if (m_lifetime.GetElapsedTime().asSeconds() >= m_lifeExpectancy)
        {
            m_system->RemoveParticle(this, m_id);
        }
        else
        {
            auto t = m_body->getGlobalPose();
            auto aT = Translation3f(Math<float>::PxToEigen(t.p));
		    auto aR = Math<float>::PxToEigen(t.q);

            auto a = aT * aR;
            *m_transform = a.matrix().transpose();
        }
    }

}