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
        SAMPLER_STATE        = 11,

        CUSTOM               = 1 << 7,

        UNDEFINED            = 255
    };

    static const char* ResourceTypeToString(ResourceType type)
    {
        switch (type)
        {
        case ResourceType::MATERIAL: return "MATERIAL";
        case ResourceType::MESH: return "MESH";
        case ResourceType::INPUT_LAYOUT: return "INPUT_LAYOUT";
        case ResourceType::PIPELINE_STATE: return "PIPELINE_STATE";
        case ResourceType::SHADER_BINARY: return "SHADER_BINARY";
        case ResourceType::SHADER_CODE: return "SHADER_CODE";
        case ResourceType::SPLINE: return "SPLINE";
        case ResourceType::TEXTURE: return "TEXTURE";
        case ResourceType::TEXTURE_REFERENCE: return "TEXTURE_REFERENCE";
        case ResourceType::SAMPLER_STATE: return "SAMPLER_STATE";
        case ResourceType::CUSTOM: return "CUSTOM";
        case ResourceType::UNDEFINED: return "UNDEFINED";        
        default: return "Invalid";
        }
    }

}