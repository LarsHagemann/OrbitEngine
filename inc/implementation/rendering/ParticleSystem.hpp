#pragma once
#include "implementation/engine/GameObject.hpp"

namespace orbit
{

    struct ParticleSystemDesc
    {
        Vector3f (*velocityFunc)() = nullptr;
        bool cyclic = true;
        uint32_t numParticlesPerSecond;
        uint32_t durationInMilliseconds;
    };

    class ParticleSystem : public GameObject
    {
        
    };
    
}