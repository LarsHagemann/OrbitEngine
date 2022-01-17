#pragma once
#include <cstdint>

namespace orbit
{

    enum class EBlendOperation : int8_t
    {
        BLEND_ADD = 1,
        BLEND_SUBTRACT = 2,
        BLEND_RSUBTRACT = 3,
        BLEND_MIN = 4,
        BLEND_MAX = 5
    };

    static const char* EBlendOperationToString(EBlendOperation op)
    {
        switch (op)
        {
        case EBlendOperation::BLEND_ADD: return "ADD";
        case EBlendOperation::BLEND_SUBTRACT: return "SUBTRACT";
        case EBlendOperation::BLEND_RSUBTRACT: return "REVERSE SUBTRACT";
        case EBlendOperation::BLEND_MIN: return "MIN";
        case EBlendOperation::BLEND_MAX: return "MAX";
        default: return "Invalid";
        }
    }

    enum class EBlend : int8_t
    {
        BLEND_ZERO	= 1,
        BLEND_ONE	= 2,
        BLEND_SRC_COLOR	= 3,
        BLEND_INV_SRC_COLOR	= 4,
        BLEND_SRC_ALPHA	= 5,
        BLEND_INV_SRC_ALPHA	= 6,
        BLEND_DEST_ALPHA	= 7,
        BLEND_INV_DEST_ALPHA	= 8,
        BLEND_DEST_COLOR	= 9,
        BLEND_INV_DEST_COLOR	= 10,
        BLEND_SRC_ALPHA_SAT	= 11,
        BLEND_BLEND_FACTOR	= 14,
        BLEND_INV_BLEND_FACTOR	= 15,
        BLEND_SRC1_COLOR	= 16,
        BLEND_INV_SRC1_COLOR	= 17,
        BLEND_SRC1_ALPHA	= 18,
        BLEND_INV_SRC1_ALPHA	= 19
    };

    static const char* EBlendToString(EBlend blend)
    {
        switch (blend)
        {
        case EBlend::BLEND_ZERO: return "BLEND_ZERO";
        case EBlend::BLEND_ONE: return "BLEND_ONE";
        case EBlend::BLEND_SRC_COLOR: return "BLEND_SRC_COLOR";
        case EBlend::BLEND_INV_SRC_COLOR: return "BLEND_INV_SRC_COLOR";
        case EBlend::BLEND_SRC_ALPHA: return "BLEND_SRC_ALPHA";
        case EBlend::BLEND_INV_SRC_ALPHA: return "BLEND_INV_SRC_ALPHA";
        case EBlend::BLEND_DEST_ALPHA: return "BLEND_DEST_ALPHA";
        case EBlend::BLEND_INV_DEST_ALPHA: return "BLEND_INV_DEST_ALPHA";
        case EBlend::BLEND_DEST_COLOR: return "BLEND_DEST_COLOR";
        case EBlend::BLEND_INV_DEST_COLOR: return "BLEND_INV_DEST_COLOR";
        case EBlend::BLEND_SRC_ALPHA_SAT: return "BLEND_SRC_ALPHA_SAT";
        case EBlend::BLEND_BLEND_FACTOR: return "BLEND_BLEND_FACTOR";
        case EBlend::BLEND_INV_BLEND_FACTOR: return "BLEND_INV_BLEND_FACTOR";
        case EBlend::BLEND_SRC1_COLOR: return "BLEND_SRC1_COLOR";
        case EBlend::BLEND_INV_SRC1_COLOR: return "BLEND_INV_SRC1_COLOR";
        case EBlend::BLEND_SRC1_ALPHA: return "BLEND_SRC1_ALPHA";
        case EBlend::BLEND_INV_SRC1_ALPHA: return "BLEND_INV_SRC1_ALPHA";
        default: return "Invalid";
        }
    }

    enum class EChannel : int8_t
    {
        CHANNEL_RED = 1,
        CHANNEL_GREEN = 2,
        CHANNEL_BLUE = 4,
        CHANNEL_ALPHA = 8,

        CHANNEL_ALL = 15
    };

    static const char* EChannelToString(EChannel blend)
    {
        switch (blend)
        {
        case EChannel::CHANNEL_RED: return "RED";
        case EChannel::CHANNEL_GREEN: return "GREEN";
        case EChannel::CHANNEL_BLUE: return "BLUE";
        case EChannel::CHANNEL_ALPHA: return "ALPHA";
        case EChannel::CHANNEL_ALL: return "ALL";
        default: return "Invalid";
        }
    }

}