#include "interfaces/rendering/Shader.hpp"
#include "implementation/misc/ShaderType.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    bool IShaderBase::LoadImpl(std::ifstream* stream)
    {
        auto rType = ENGINE->RMGetResourceType(GetId());
        ShaderType type;
        stream->read((char*)&type, sizeof(ShaderType));
        auto ReadString = [&]() {
            uint32_t strLen = 0u;
            stream->read((char*)&strLen, sizeof(uint32_t));
            std::string str;
            str.resize(strLen);
            stream->read(str.data(), strLen);
            return str;
        };
        auto ReadLongString = [&]() {
            uint64_t strLen = 0u;
            stream->read((char*)&strLen, sizeof(uint64_t));
            std::string str;
            str.resize(strLen);
            stream->read(str.data(), strLen);
            return str;
        };
        switch (rType)
        {
        case ResourceType::SHADER_BINARY: {
            // can be one of the following:
            //  - binary code (length of bytecode + bytecode)
            //  - reference to binary code (length of path + path)
            switch (type)
            {
            case orbit::ShaderType::SHADER_VERTEX:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_PIXEL:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_DOMAIN:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_HULL:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_GEOMETRY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_COMPUTE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_VERTEX_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_PIXEL_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_DOMAIN_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_HULL_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_GEOMETRY_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_COMPUTE_BINARY: {
                // binary code (length of bytecode + bytecode)
                auto binary = ReadLongString();
                LoadBinary(binary.data(), binary.length());
                break;
            }
            case orbit::ShaderType::SHADER_VERTEX_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_PIXEL_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_DOMAIN_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_HULL_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_GEOMETRY_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_COMPUTE_REFERENCE: {
                // reference to binary code (length of path + path)
                auto path = ReadString();
                LoadBinary(path);
                break;
            }
            }
            break;
        }
        case ResourceType::SHADER_CODE: {
            // can be one of the following:
            //  - shader code (length of code + code)
            //  - reference to code (length of path + path)
            //  - compiled shader code (length of bytecode + bytecode)
            switch (type)
            {
            case orbit::ShaderType::SHADER_VERTEX:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_PIXEL:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_DOMAIN:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_HULL:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_GEOMETRY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_COMPUTE: {
                // shader code (length of code + code)
                auto shader_model = ReadString();
                std::string entry_function = "";
                if (shader_model != "glsl")
                    entry_function = ReadString();
                auto code = ReadLongString();
                ComPtr<ID3DBlob> binary;
                ComPtr<ID3DBlob> error;
                auto result = D3DCompile(
                    code.c_str(),
                    code.length(),
                    "shader",
                    nullptr,
                    D3D_COMPILE_STANDARD_FILE_INCLUDE,
                    entry_function.c_str(),
                    shader_model.c_str(),
                    0,
                    0,
                    binary.ReleaseAndGetAddressOf(),
                    error.ReleaseAndGetAddressOf()
                );
                if (FAILED(result))
                    ORBIT_THROW("Failed to compile shader from orb file");

                LoadBinary(static_cast<const char*>(binary->GetBufferPointer()), binary->GetBufferSize());
                break;
            }
            case orbit::ShaderType::SHADER_VERTEX_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_PIXEL_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_DOMAIN_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_HULL_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_GEOMETRY_BINARY:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_COMPUTE_BINARY: {
                // binary code (length of bytecode + bytecode)
                auto binary = ReadLongString();
                LoadBinary(binary.data(), binary.length());
                break;
            }
            case orbit::ShaderType::SHADER_VERTEX_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_PIXEL_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_DOMAIN_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_HULL_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_GEOMETRY_REFERENCE:
                [[fallthrough]];
            case orbit::ShaderType::SHADER_COMPUTE_REFERENCE: {
                // reference to code (length of path + path)
                auto shader_model = ReadString();
                std::string entry_function = "";
                if (shader_model != "glsl")
                    entry_function = ReadString();
                auto path = ReadString();
                CompileFile(path, entry_function);
                break;
            }
            }
            break;
        }
        }
        return true;
    }

}