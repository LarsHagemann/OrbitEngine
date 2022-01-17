#pragma once
#include <unordered_set>
#include <vector>

#include "implementation/engine/GameObject.hpp"
#include "implementation/backends/impl/VertexBufferImpl.hpp"

namespace orbit
{

    namespace PSDirectionFunctions
    {
        extern Vector3f randomOnUnitSphere();
    }

    namespace PSVelocityDistribution
    {
        extern float constant1();
        extern float random();
    }

    namespace PSLifetimeDistribution = PSVelocityDistribution;

    struct ParticleSystemDesc
    {
        // @brief: generator function for travelling direction.
        //  this function must return normalized vectors as directions
        // @default: returns a random point on the unit sphere
        Vector3f (*directionFunc)() = PSDirectionFunctions::randomOnUnitSphere;
        // @brief: distribution function of the velocities. 
        //  Must return values between 0.f and 1.f
        // @default: always return 1
        float (*velocityDistribution)() = PSVelocityDistribution::constant1;
        // @brief: scales the velocity up or down
        // @default: 10 meters per second
        float velocityScale = 10.f;
        // @brief: distribution for the lifetime of a particle.
        //  Must return values between 0.f and 1.f
        // @default: always return 1
        float (*lifetimeDistribution)() = PSLifetimeDistribution::constant1;
        // @brief: scales the lifetime up or down
        // @default: 2 seconds
        float lifetimeScale = 2.f;
        // @brief: true if the animation does not stop
        // @default: false
        bool cyclic = false;
        // @brief: Approximate number of particles to be spawned each second
        // @default: 100 particles per second
        uint32_t numParticlesPerSecond = 100;
        // @brief: Duration of the particle effect. Only if cyclic=false
        // @default: 5 seconds
        uint32_t durationInMilliseconds = 5000;
    };

    class ParticleSystem : public GameObject
    {
    private:
        VertexBuffer<Matrix4f> m_transforms;
    private:
        friend class Particle;
        void RemoveParticle(Particle* particle) const;
    public:
        ParticleSystem(const ParticleSystemDesc& desc);
        void Start();
        void Stop();
    };
    
}