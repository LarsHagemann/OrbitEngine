#pragma once
#include <cstdint>

namespace orbit
{

    enum class MaterialFlag : uint32_t
    {
        FLAG_HAS_ALBEDO_TEXTURE    = (1 << 0),
        FLAG_HAS_NORMAL_MAP        = (1 << 1),
        FLAG_HAS_ROUGHNESS_TEXTURE = (1 << 2),
        FLAG_HAS_OCCLUSION_MAP     = (1 << 3),
    };

}
