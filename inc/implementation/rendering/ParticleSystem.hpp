#pragma once
#include <unordered_set>
#include <vector>

#include "implementation/engine/GameObject.hpp"
#include "implementation/backends/impl/VertexBufferImpl.hpp"
#include "implementation/rendering/Particle.hpp"
#include "implementation/rendering/Mesh.hpp"
#include "implementation/rendering/Vertex.hpp"

namespace orbit
{

    class PSDirectionFunctions
    {
    public:
        static Vector3f randomOnUnitSphere();
    };

    class PSVelocityDistribution
    {
    public:
        static float constant1();
        static float random();
    };

    using PSLifetimeDistribution = PSVelocityDistribution;

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
        float velocityScale = .01f;
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
        // @brief: The mesh to be rendered as particles
        ResourceId particleMesh;
    };

    class ParticleSystem : public GameObject
    {
    private:
        UPtr<VertexBuffer<Matrix4f>> m_transforms;
        std::unordered_set<Particle*> m_particles;
        mutable std::vector<Particle*> m_scheduledForRemoval;
        mutable std::vector<size_t> m_freeTransforms;

        float m_elapsedAccumulated;
        float m_timePerParticle;
        float m_particleLifetimeScale;
        float m_velocityScale;
        float (*m_velocityDistribution)();
        float (*m_lifetimeDistribution)();
        Vector3f (*m_directionFunc)();

        Clock m_elapsed;
        uint32_t m_duration;
        bool m_running;
        bool m_cyclic;
        ResourceId m_particleMesh = 0;
    private:
        friend class Particle;
        void RemoveParticle(Particle* particle, size_t id) const;
    public:
        ParticleSystem(const ParticleSystemDesc& desc);
        void Start();
        void Stop();
        virtual bool LoadImpl(std::ifstream* stream) override;
        virtual void UnloadImpl() override;
        void Init() override {}
        void LoadFromDesc(const ParticleSystemDesc& desc);
        virtual void Update(const Time& dt) override;
        virtual void Draw() const override;
    };
    
}