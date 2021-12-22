#pragma once
#include <cstdint>

namespace orbit
{

    enum class FormatType : uint8_t
    {
        FORMAT_FLOAT1 = 0,
        FORMAT_FLOAT2 = 1,
        FORMAT_FLOAT3 = 2,
        FORMAT_FLOAT4 = 3,
    };

    static const char* FormatTypeToString(FormatType f) 
    {
        switch (f) {
        case FormatType::FORMAT_FLOAT1: return "FORMAT_FLOAT1";
        case FormatType::FORMAT_FLOAT2: return "FORMAT_FLOAT2";
        case FormatType::FORMAT_FLOAT3: return "FORMAT_FLOAT3";
        case FormatType::FORMAT_FLOAT4: return "FORMAT_FLOAT4";
        }

        return "_Unknown_";
    }

}