#pragma once
#include "implementation/misc/Transform.hpp"
#include "implementation/misc/Time.hpp"

#include <PxRigidDynamic.h>

namespace orbit
{

    class ParticleSystem;

    struct ParticleDesc
    {
        const ParticleSystem* system;
        Matrix4f* transform;
        size_t id;
        float lifeExpectancy;
        PxRigidDynamic* body;
    };

    class Particle
    {
    private:
        friend ParticleSystem;

        const ParticleSystem* m_system;
        Matrix4f* m_transform;
        PxRigidDynamic* m_body;
        size_t m_id;

        float m_lifeExpectancy;
        Clock m_lifetime;
    public:
        Particle(const ParticleDesc& desc);
        ~Particle();

        void Update(const Time& dt);
    };

}