#pragma once
#include "implementation/misc/Transform.hpp"

namespace orbit
{

    class ParticleSystem;

    class Particle
    {
    private:
        const ParticleSystem& m_system;
        Matrix4f* m_transform;
    public:
        Particle(const ParticleSystem& system, Matrix4f* transform);
    };

}