#pragma once
#include <cstdint>

namespace orbit
{

    enum class InputLayoutDataType : uint8_t
    {
        VERTEX_DATA,
        INSTANCE_DATA
    };

    static const char* InputLayoutDataTypeToString(InputLayoutDataType t)
    {
        switch (t) {
        case InputLayoutDataType::VERTEX_DATA: return "VERTEX_DATA";
        case InputLayoutDataType::INSTANCE_DATA: return "INSTANCE_DATA";
        }
        return "_Unknown_";
    }

}