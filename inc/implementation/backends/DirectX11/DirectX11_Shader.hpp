#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "implementation/misc/Logger.hpp"
#include "interfaces/rendering/Shader.hpp"

namespace orbit
{

    template<typename ShaderInterfaceType>
    class DirectX11Shader : public IShaderBase
    {
    protected:
        ComPtr<ShaderInterfaceType> m_buffer;
        std::string m_shaderModel;
    protected:
        virtual HRESULT CreateShader(const char* binary, size_t binarySize) = 0;
    protected:
        DirectX11Shader(const std::string& shaderModel) :
            m_shaderModel(shaderModel)
        {
        }
    public:
        bool CompileFile(const fs::path& path, const std::string& entry_point) override
        {
            ComPtr<ID3DBlob> error;
            ComPtr<ID3DBlob> binary;
            D3D_SHADER_MACRO macros[] = {
                { "ORBIT_DIRECTX_11", "1" },
                { nullptr, nullptr }
            };
            auto result = D3DCompileFromFile(
                path.c_str(),
                macros,
                D3D_COMPILE_STANDARD_FILE_INCLUDE,
                entry_point.c_str(),
                m_shaderModel.c_str(),
                0,
                0,
                binary.ReleaseAndGetAddressOf(),
                error.ReleaseAndGetAddressOf()
            );
            if (FAILED(result))
            {
                if (error)
                {
                    ORBIT_ERROR(
                        "Failed to compile file '%s': %s",
                        fs::absolute(path).generic_string().c_str(),
                        error->GetBufferPointer(),
                        error->GetBufferSize()
                    );
                }
                else
                {
                    ORBIT_ERROR_HR(result, "Failed to compile file '%s'", fs::absolute(path).generic_string().c_str());
                }
                return false;
            }

            result = CreateShader(static_cast<const char*>(binary->GetBufferPointer()), binary->GetBufferSize());
            if (FAILED(result))
                ORBIT_ERROR_HR(result, "Failed to create vertex shader");
            return SUCCEEDED(result);
        }
        bool LoadBinary(const fs::path& path) override
        {
            ComPtr<ID3DBlob> binary;
            auto result = D3DReadFileToBlob(
                path.c_str(),
                binary.ReleaseAndGetAddressOf()
            );
            if (FAILED(result))
            {
                ORBIT_ERROR("Failed to load shader binary from %s", path.generic_string().c_str());
                return false;
            }

            result = CreateShader(static_cast<const char*>(binary->GetBufferPointer()), binary->GetBufferSize());
            if (FAILED(result))
                ORBIT_ERROR_HR(result, "Failed to load shader");
            return SUCCEEDED(result);
        }
        bool LoadBinary(const char* binary, uint32_t binaryLen) override
        {
            auto result = CreateShader(binary, binaryLen);
            if (FAILED(result))
                ORBIT_ERROR_HR(result, "Failed to load shader");
            return SUCCEEDED(result);
        }
        void UnloadImpl() override 
        {
            m_buffer = nullptr;
        }
    };

}
#endif