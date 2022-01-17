#pragma once
#include <cstdint>

namespace orbit
{

    enum class CullMode : uint8_t
    {
        CULL_NONE,
        CULL_BACK,
        CULL_FRONT
    };

    static const char* CullModeToString(CullMode c)
    {
        switch (c)
        {
        case CullMode::CULL_NONE: return "NONE";
        case CullMode::CULL_BACK: return "BACK";
        case CullMode::CULL_FRONT: return "FRONT";
        }

        return "_Unknown_";
    }

    enum class FillMode : uint8_t
    {
        FILL_SOLID,
        FILL_WIREFRAME
    };

    static const char* FillModeToString(FillMode f)
    {
        switch (f)
        {
        case FillMode::FILL_SOLID: return "SOLID";
        case FillMode::FILL_WIREFRAME: return "WIREFRAME";
        }

        return "_Unknown_";
    }

}