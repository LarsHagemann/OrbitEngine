#pragma once

namespace orbit
{

    enum EPrimitiveType : uint8_t
    {
        TRIANGLES,
        TRIANGLE_STRIP,
        TRIANGLE_FAN,
        LINES,
        POINTS
    };

    static const char* EPrimitiveTypeToString(EPrimitiveType type)
    {
        switch (type)
        {
        case EPrimitiveType::TRIANGLES: return "TRIANGLES";
        case EPrimitiveType::TRIANGLE_STRIP: return "TRIANGLE_STRIP";
        case EPrimitiveType::TRIANGLE_FAN: return "TRIANGLE_FAN";
        case EPrimitiveType::LINES: return "LINES";
        case EPrimitiveType::POINTS: return "POINTS";
        default: return "Invalid";
        }
    }

}