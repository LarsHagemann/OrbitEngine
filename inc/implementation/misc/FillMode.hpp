#pragma once
#include <cstdint>

namespace orbit
{

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