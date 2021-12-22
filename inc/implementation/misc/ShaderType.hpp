#pragma once
#include <cstdint>

namespace orbit
{

    enum class ShaderType : uint8_t
    {
        SHADER_VERTEX = 0,
        SHADER_PIXEL,
        SHADER_DOMAIN,
        SHADER_HULL,
        SHADER_GEOMETRY,
        SHADER_COMPUTE,
        NUM_SHADER_TYPES,
        SHADER_VERTEX_BINARY,
        SHADER_PIXEL_BINARY,
        SHADER_DOMAIN_BINARY,
        SHADER_HULL_BINARY,
        SHADER_GEOMETRY_BINARY,
        SHADER_COMPUTE_BINARY,
        SHADER_VERTEX_REFERENCE,
        SHADER_PIXEL_REFERENCE,
        SHADER_DOMAIN_REFERENCE,
        SHADER_HULL_REFERENCE,
        SHADER_GEOMETRY_REFERENCE,
        SHADER_COMPUTE_REFERENCE,
    };

    constexpr static ShaderType GetReferenceType(ShaderType shader_type)
    {
        return static_cast<ShaderType>(
            static_cast<uint8_t>(shader_type) + 2 * static_cast<uint8_t>(ShaderType::NUM_SHADER_TYPES) + 1
        );
    }

    constexpr static ShaderType GetBinaryType(ShaderType shader_type)
    {
        return static_cast<ShaderType>(
            static_cast<uint8_t>(shader_type) + static_cast<uint8_t>(ShaderType::NUM_SHADER_TYPES) + 1
        );
    }

    static const char* ShaderTypeToString(ShaderType s)
    {
        switch (s) {
        case ShaderType::SHADER_VERTEX: return "SHADER_VERTEX";
        case ShaderType::SHADER_PIXEL: return "SHADER_PIXEL";
        case ShaderType::SHADER_DOMAIN: return "SHADER_DOMAIN";
        case ShaderType::SHADER_HULL: return "SHADER_HULL";
        case ShaderType::SHADER_GEOMETRY: return "SHADER_GEOMETRY";
        case ShaderType::SHADER_COMPUTE: return "SHADER_COMPUTE";
        case ShaderType::SHADER_VERTEX_BINARY: return "SHADER_VERTEX_BINARY";
        case ShaderType::SHADER_PIXEL_BINARY: return "SHADER_PIXEL_BINARY";
        case ShaderType::SHADER_DOMAIN_BINARY: return "SHADER_DOMAIN_BINARY";
        case ShaderType::SHADER_HULL_BINARY: return "SHADER_HULL_BINARY";
        case ShaderType::SHADER_GEOMETRY_BINARY: return "SHADER_GEOMETRY_BINARY";
        case ShaderType::SHADER_COMPUTE_BINARY: return "SHADER_COMPUTE_BINARY";
        case ShaderType::SHADER_VERTEX_REFERENCE: return "SHADER_VERTEX_REFERENCE";
        case ShaderType::SHADER_PIXEL_REFERENCE: return "SHADER_PIXEL_REFERENCE";
        case ShaderType::SHADER_DOMAIN_REFERENCE: return "SHADER_DOMAIN_REFERENCE";
        case ShaderType::SHADER_HULL_REFERENCE: return "SHADER_HULL_REFERENCE";
        case ShaderType::SHADER_GEOMETRY_REFERENCE: return "SHADER_GEOMETRY_REFERENCE";
        case ShaderType::SHADER_COMPUTE_REFERENCE: return "SHADER_COMPUTE_REFERENCE";
        }
        
        return "_Unknown_";
    }

}