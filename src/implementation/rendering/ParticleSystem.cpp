#include "implementation/rendering/ParticleSystem.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    namespace PSDirectionFunctions
    {
        Vector3f randomOnUnitSphere()
        {
            static const auto dist = std::uniform_real_distribution<float>{ -Math<float>::TWO_PI, Math<float>::TWO_PI };
            auto theta0 = ENGINE->NextRandomValue<float>(dist);
            auto theta1 = ENGINE->NextRandomValue<float>(dist);
            return Vector3f{
                sin(theta0) * sin(theta1),
                cos(theta1),
                cos(theta0) * sin(theta1)
            };
        }
    }

    namespace PSVelocityDistribution
    {
        float constant1()
        {
            return 1.f;
        }

        float random()
        {
            static const auto dist = std::uniform_real_distribution<float>{ 0.f, 1.f };
            return ENGINE->NextRandomValue<float>(dist);
        }

    }

}