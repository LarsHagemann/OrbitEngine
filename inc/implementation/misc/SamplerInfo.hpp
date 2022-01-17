#pragma once
#include <cstdint>

namespace orbit
{

    enum class ETextureAddress : uint8_t
    {
        WRAP = 1,
        MIRROR = 4,
        CLAMP = 2,
        BORDER = 3
    };

    static const char* ETextureAddressToString(ETextureAddress address)
    {
        switch (address)
        {
        case ETextureAddress::WRAP: return "WRAP";
        case ETextureAddress::MIRROR: return "MIRROR";
        case ETextureAddress::CLAMP: return "CLAMP";
        case ETextureAddress::BORDER: return "BORDER";
        default:                     return "Invalid";
        }
    }

    enum class ESamplerFilter : uint8_t
    {
        MIN_MAG_MIP_POINT = 1,
        MIN_MAG_POINT_MIP_LINEAR = 2,
        MIN_POINT_MAG_LINEAR_MIP_POINT = 3,
        MIN_POINT_MAG_MIP_LINEAR = 4,
        MIN_LINEAR_MAG_MIP_POINT = 5,
        MIN_LINEAR_MAG_POINT_MIP_LINEAR = 6,
        MIN_MAG_LINEAR_MIP_POINT = 7,
        MIN_MAG_MIP_LINEAR = 8,
        ANISOTROPIC = 9,
    };

    static const char* ESamplerFilterToString(ESamplerFilter filter)
    {
        switch (filter)
        {
        case ESamplerFilter::MIN_MAG_MIP_POINT: return "MIN_MAG_MIP_POINT";
        case ESamplerFilter::MIN_MAG_POINT_MIP_LINEAR: return "MIN_MAG_POINT_MIP_LINEAR";
        case ESamplerFilter::MIN_POINT_MAG_LINEAR_MIP_POINT: return "MIN_POINT_MAG_LINEAR_MIP_POINT";
        case ESamplerFilter::MIN_POINT_MAG_MIP_LINEAR: return "MIN_POINT_MAG_MIP_LINEAR";
        case ESamplerFilter::MIN_LINEAR_MAG_MIP_POINT: return "MIN_LINEAR_MAG_MIP_POINT";
        case ESamplerFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR: return "MIN_LINEAR_MAG_POINT_MIP_LINEAR";
        case ESamplerFilter::MIN_MAG_LINEAR_MIP_POINT: return "MIN_MAG_LINEAR_MIP_POINT";
        case ESamplerFilter::MIN_MAG_MIP_LINEAR: return "MIN_MAG_MIP_LINEAR";
        case ESamplerFilter::ANISOTROPIC: return "ANISOTROPIC";
        default: return "Invalid";
        }
    }

}
