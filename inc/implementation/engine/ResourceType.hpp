#pragma once
#include <cstdint>

namespace orbit
{

    enum class ResourceType : uint8_t
    {
        MATERIAL             = 0,
        MESH                 = 1,
        INPUT_LAYOUT         = 2,
        PIPELINE_STATE       = 3,
        SHADER_BINARY        = 4,
        SHADER_CODE          = 5,
        SPLINE               = 6,
        TEXTURE              = 7,
        TEXTURE_REFERENCE    = 8,
        RASTERIZER_STATE     = 9,
        BLEND_STATE          = 10,

        CUSTOM               = 1 << 7,

        UNDEFINED            = 255
    };

}