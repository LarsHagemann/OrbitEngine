#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_InputLayout.hpp"
#include "implementation/misc/FormatType.hpp"
#include "implementation/misc/InputLayoutDataType.hpp"
#include "implementation/engine/Engine.hpp"
#include "implementation/backends/impl/VertexShaderImpl.hpp"

#include <fstream>
#include <d3dcompiler.h>

namespace orbit
{

    bool DirectX11InputLayout::LoadImpl(std::ifstream* stream)
    {
        uint32_t numElements;
        stream->read((char*)&numElements, sizeof(uint32_t));
        std::vector<D3D11_INPUT_ELEMENT_DESC> elements;
        std::vector<std::string> semantic_names;
        elements.resize(numElements);
        semantic_names.resize(numElements);
        for (auto i = 0u; i < numElements; ++i)
        {
            uint32_t nameLen = 0u;
            FormatType format;
            InputLayoutDataType datatype;
            uint32_t semantic_index = 0u;
            uint32_t slot = 0u;
            stream->read((char*)&nameLen, sizeof(uint32_t));
            semantic_names.at(i).resize(nameLen);
            stream->read(semantic_names.at(i).data(), nameLen);
            stream->read((char*)&format, sizeof(FormatType));
            stream->read((char*)&datatype, sizeof(InputLayoutDataType));
            stream->read((char*)&semantic_index, sizeof(uint32_t));
            stream->read((char*)&slot, sizeof(uint32_t));
            DXGI_FORMAT dxgi_format;
            D3D11_INPUT_CLASSIFICATION input_type;
            switch (format)
            {
            case FormatType::FORMAT_FLOAT1:
                dxgi_format = DXGI_FORMAT_R32_FLOAT;
                break;
            case FormatType::FORMAT_FLOAT2:
                dxgi_format = DXGI_FORMAT_R32G32_FLOAT;
                break;
            case FormatType::FORMAT_FLOAT3:
                dxgi_format = DXGI_FORMAT_R32G32B32_FLOAT;
                break;
            case FormatType::FORMAT_FLOAT4:
                dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
                break;
            }

            auto instanceDataStepRate = 0u;
            switch (datatype)
            {
            case InputLayoutDataType::VERTEX_DATA:
                input_type = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA;
                break;
            case InputLayoutDataType::INSTANCE_DATA:
                input_type = D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA;
                instanceDataStepRate = 1;
                break;
            }
            elements[i] = { semantic_names.at(i).c_str(), semantic_index, dxgi_format, slot, D3D11_APPEND_ALIGNED_ELEMENT, input_type, instanceDataStepRate };
        }

        // Create shader:
        std::string dummyShader;
        std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
        uint32_t i = 0u;
        dummyShader += "struct vsin {\n";
        for (const auto& element : elements)
        {
            int32_t j = i++;
            std::string identifier;
            while (j >= 0)
            {
                identifier += alphabet[j % alphabet.size()];
                j -= alphabet.size();
            }
            dummyShader += '\t';
            switch (element.Format)
            {
            case DXGI_FORMAT_R32_FLOAT: dummyShader += "float "; break;
            case DXGI_FORMAT_R32G32_FLOAT: dummyShader += "float2 "; break;
            case DXGI_FORMAT_R32G32B32_FLOAT: dummyShader += "float3 "; break;
            case DXGI_FORMAT_R32G32B32A32_FLOAT: dummyShader += "float4 "; break;
            }
            dummyShader += identifier + " : " + element.SemanticName + std::to_string(element.SemanticIndex) + ";\n";

        }
        dummyShader += "}; \nfloat4 main(vsin input) : SV_POSITION \n{ \n\treturn float4(0, 0, 0, 0); \n}";
        ComPtr<ID3DBlob> binary;
        ComPtr<ID3DBlob> error;
        auto result = D3DCompile(
            dummyShader.data(),
            dummyShader.length(),
            "dummy_shader",
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            "main",
            "vs_5_0",
            0,
            0,
            binary.ReleaseAndGetAddressOf(),
            error.ReleaseAndGetAddressOf()
        );
        if (FAILED(result)) {
            ORBIT_THROW("Failed to create dummy shader");
        }

        result = ENGINE->Device()->CreateInputLayout(
            elements.data(), 
            numElements, 
            binary->GetBufferPointer(), 
            binary->GetBufferSize(), 
            m_inputLayout.ReleaseAndGetAddressOf()
        );
        if (FAILED(result))
        {
            ORBIT_ERROR_HR(result, "Failed to create input layout");
            return false;
        }

        return true;
    }

    void DirectX11InputLayout::UnloadImpl()
    {
        m_inputLayout = nullptr;
    }

    void DirectX11InputLayout::Bind() const
    {
#ifdef _DEBUG
        if (!m_inputLayout)
        {
            ORBIT_ERROR("You are trying to bind an unloaded input layout");
            return;
        }
#endif
        ENGINE->Context()->IASetInputLayout(m_inputLayout.Get());
    }

}
#endif