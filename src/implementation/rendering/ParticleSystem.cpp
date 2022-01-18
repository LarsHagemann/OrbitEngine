#include "implementation/rendering/ParticleSystem.hpp"
#include "implementation/engine/Engine.hpp"

#include <PxRigidDynamic.h>

#include <numeric>

namespace orbit
{

    Vector3f PSDirectionFunctions::randomOnUnitSphere()
    {
        static auto dist = std::uniform_real_distribution<float>{ -Math<float>::TWO_PI, Math<float>::TWO_PI };
        auto theta0 = ENGINE->NextRandomValue<float>(dist);
        auto theta1 = ENGINE->NextRandomValue<float>(dist);
        return Vector3f{
            sin(theta0) * sin(theta1),
            cos(theta1),
            cos(theta0) * sin(theta1)
        };
    }

    float PSVelocityDistribution::constant1()
    {
        return 1.f;
    }

    float PSVelocityDistribution::random()
    {
        static auto dist = std::uniform_real_distribution<float>{ 0.f, 1.f };
        return ENGINE->NextRandomValue<float>(dist);
    }

    ParticleSystem::ParticleSystem(const ParticleSystemDesc& desc) :
        m_elapsedAccumulated(0.f),
        m_timePerParticle(0.f),
        m_running(false)
    {
        LoadFromDesc(desc);
    }

    void ParticleSystem::RemoveParticle(Particle* particle, size_t id) const
    {
        m_scheduledForRemoval.emplace_back(particle);
        m_freeTransforms.emplace_back(id);
    }

    void ParticleSystem::Start()
    {
        m_running = true;
    }

    void ParticleSystem::Stop()
    {
        m_running = false;
    }

    void ParticleSystem::Update(const Time& dt)
    {
        GameObject::Update(dt);
        for (auto particle : m_scheduledForRemoval)
        {
            m_particles.erase(particle);
            ENGINE->GetPhysXScene()->removeActor(*particle->m_body);
            particle->m_body->release();
            delete particle;
            particle = nullptr;
        }
        m_scheduledForRemoval.clear();

        for (auto particle : m_particles)
        {
            // Update particles
            particle->Update(dt);
        }

        if (m_running)
        {
            m_elapsedAccumulated += dt.asSeconds();
            while (m_elapsedAccumulated >= m_timePerParticle && !m_freeTransforms.empty())
            {
                auto body = ENGINE->GetPhysics()->createRigidDynamic(PxTransform(
                    Math<float>::EigenToPx3(Vector3f::Zero()),
                    Math<float>::EigenToPx(Quaternionf::Identity())));
                ENGINE->GetPhysXScene()->addActor(*body);

                auto id = m_freeTransforms.back();
                m_freeTransforms.pop_back();

                auto lifetime = m_particleLifetimeScale * m_lifetimeDistribution();
                auto velocity = m_velocityScale * m_velocityDistribution() * m_directionFunc();

                auto buffer = m_transforms->GetVertices().data() + id;
                *buffer = Matrix4f::Identity();
                ParticleDesc desc;
                desc.system = this;
                desc.transform = buffer;
                desc.id = id;
                desc.lifeExpectancy = lifetime;
                desc.body = body;

                body->setLinearVelocity(Math<float>::EigenToPx3(velocity));

                auto particle = new Particle(desc);
                m_particles.emplace(particle);
                m_elapsedAccumulated -= m_timePerParticle;
            }

            if (!m_cyclic && m_elapsed.GetElapsedTime().asMilliseconds() >= m_duration)
            {
                Stop();
            }
        }
    }

    void ParticleSystem::Draw() const
    {
        GameObject::Draw();
        if (m_running || !m_particles.empty())
        {

            m_transforms->UpdateBuffer();
            m_transforms->Bind(1, sizeof(Matrix4f), 0);

            auto mesh = ENGINE->RMLoadResource<Mesh<Vertex>>(m_particleMesh);

            mesh->Bind();
            mesh->Draw(m_transforms->NumVertices());
        }
    }

    bool ParticleSystem::LoadImpl(std::ifstream* stream)
    {
        if (stream == nullptr)
            return true;

        ParticleSystemDesc desc;

        LoadFromDesc(desc);
        return true;
    }

    void ParticleSystem::UnloadImpl()
    {
        for (auto particle : m_particles)
        {
            delete particle;
            particle = nullptr;
        }
        m_particles.clear();
        m_scheduledForRemoval.clear();
        m_transforms = nullptr;
    }

    void ParticleSystem::LoadFromDesc(const ParticleSystemDesc& desc)
    {
        // A particle lives at most x=desc.lifetimeScale seconds
        // and in those x seconds x*y with y=desc.numParticlesPerSecond particles
        // can be spawned
        auto maxNbParticles = desc.lifetimeScale * desc.numParticlesPerSecond;
        m_timePerParticle = 1.f / desc.numParticlesPerSecond;
        m_elapsedAccumulated = 0.f;
        m_transforms = std::make_unique<VertexBuffer<Matrix4f>>();
        m_transforms->ResizeBuffer(maxNbParticles);
        m_freeTransforms.resize(maxNbParticles);
        m_particleMesh = desc.particleMesh;
        m_particleLifetimeScale = desc.lifetimeScale;
        m_velocityScale = desc.velocityScale;
        m_velocityDistribution = desc.velocityDistribution;
        m_lifetimeDistribution  = desc.lifetimeDistribution;
        m_directionFunc = desc.directionFunc;
        m_cyclic = desc.cyclic;
        m_duration = desc.durationInMilliseconds;
        Vector3f (*m_directionFunc)();
        std::iota(m_freeTransforms.begin(), m_freeTransforms.end(), 0);
    }

}