#pragma once

namespace orbit
{

    struct MaterialProperties
    {
        float staticFriction;
        float dynamicFriction;
        float restitution;
        static MaterialProperties DefaultMaterial()
        {
            return MaterialProperties{
                0.5f,
                0.5f,
                0.1f
            };
        }
    };

} // namespace orbit