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

}