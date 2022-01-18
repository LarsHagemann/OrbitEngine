#pragma once
#include <string>
#include <cstdint>
#include <variant>
#include <set>
#include <unordered_map>

#include <Eigen/Dense>
#include <filesystem>

#include "implementation/engine/ResourceType.hpp"
#include "implementation/misc/ShaderType.hpp"
#include "implementation/misc/FormatType.hpp"
#include "implementation/misc/InputLayoutDataType.hpp"
#include "implementation/misc/RasterizerInfo.hpp"
#include "implementation/rendering/Light.hpp"
#include "implementation/rendering/Submesh.hpp"
#include "implementation/rendering/MaterialFlags.hpp"
#include "implementation/misc/BlendInfo.hpp"
#include "implementation/misc/SamplerInfo.hpp"
#include "implementation/misc/PrimitiveType.hpp"

namespace orbtool
{

    using namespace Eigen;
    namespace fs = std::filesystem;

    using ResourceType = orbit::ResourceType;
    using ShaderType = orbit::ShaderType;
    using FormatType = orbit::FormatType;
    using DataType = orbit::InputLayoutDataType;
    using CullMode = orbit::CullMode;
    using FillMode = orbit::FillMode;
    using Light = orbit::Light;
    using LightType = orbit::LightType;
    using SubMesh = orbit::Submesh;
    using MaterialFlag = orbit::MaterialFlag;
    using EBlendOp = orbit::EBlendOperation;
    using EBlend = orbit::EBlend;
    using EChannel = orbit::EChannel;
    using EFilter = orbit::ESamplerFilter;
    using EAddress = orbit::ETextureAddress;
    using EPrimitiveType = orbit::EPrimitiveType;

    struct OrbMaterial
    {
        float roughness = 0.f;
        Vector4f diffuse;
        Vector4f specular;
        std::string diffuseTextureId;
        std::string normalMapId;
        std::string roughnessMapId;
        std::string occlusionMapId;
    };

    struct OrbVertex
    {
        Vector3f position;
        Vector3f normal;
        Vector3f tangent;
        Vector2f textureCoords;
    };

    static bool operator==(const OrbVertex& v0, const OrbVertex& v1)
    {
        return
            v0.position == v1.position &&
            v0.normal == v1.normal &&
            v0.tangent == v1.tangent &&
            v0.textureCoords == v1.textureCoords;
    }

    struct OrbMesh
    {
        std::string material;
        std::vector<SubMesh> submeshes;
        std::vector<OrbVertex> vertices;
        std::vector<uint32_t> indices;
    };

    struct OrbTexture
    {
        fs::path texturePath;
        fs::path referencePath;
        bool onlyReference = false;
    };

    struct OrbInputLayoutElement
    {
        std::string semantic_name;
        FormatType format = FormatType::FORMAT_FLOAT3;
        DataType type = DataType::VERTEX_DATA;
        uint32_t semantic_index = 0u;
        uint32_t inputSlot = 0u;
    };

    struct OrbInputLayout
    {
        std::vector<OrbInputLayoutElement> elements;
    };

    struct OrbShaderCode
    {
        ShaderType type;
        fs::path shader_code;
        fs::path reference_path;
        std::string entry_point;
        std::string shader_model;
        std::vector<std::string> compiler_macros;
        bool compile = false;
    };

    struct OrbShaderBinary
    {
        ShaderType type;
        fs::path shader_binary;
        fs::path reference_path;
    };

    struct OrbPipelineState
    {
        std::string vShaderId; // vertex shader id
        std::string pShaderId; // pixel shader id
        std::string hShaderId; // hull shader id
        std::string dShaderId; // domain shader id
        std::string gShaderId; // geometry shader id
        std::string iLayoutId; // input layout id
        std::string rsStateId; // rasterizer state id
        std::string bsStateId; // blend state id
        std::unordered_map<uint32_t, std::string> sStateIds; // sampler states
        EPrimitiveType primitiveType = EPrimitiveType::TRIANGLES;
    };

    struct OrbRasterizerState
    {
        CullMode cull_mode = CullMode::CULL_BACK;
        FillMode fill_mode = FillMode::FILL_SOLID;
    };

    struct OrbBlendState
    {
        float blendFactor[4] = { 0.f,0.f,0.f,0.f };

        bool blendEnabled;
        bool alphaToCoverageEnabled;
        EBlendOp blendOperation;
        EBlendOp alphaBlendOperation;
        EBlend srcBlend;
        EBlend srcAlphaBlend;
        EBlend destBlend;
        EBlend destAlphaBlend;
        int8_t channelMask;
    };

    struct OrbSamplerState
    {
        EFilter filter;
        EAddress addressX1;
        EAddress addressX2;
        EAddress addressX3;
    };

    struct OrbObject
    {
        std::string name;
        std::variant<
            OrbMaterial, 
            OrbMesh, 
            OrbTexture, 
            OrbShaderCode,
            OrbInputLayout, 
            OrbPipelineState, 
            OrbShaderBinary, 
            OrbRasterizerState,
            OrbBlendState,
            OrbSamplerState> value;
    };

    static bool operator==(const OrbObject& a, const OrbObject& b)
    {
        return a.name == b.name;
    }

    static bool operator<(const OrbObject& a, const OrbObject& b)
    {
        return a.name < b.name;
    }

    class OrbIntermediate
    {
    private:
        //friend class OrbFile;
        mutable std::vector<OrbObject> m_objects;
        mutable std::unordered_map<std::string, uint32_t> m_objectIndices;
    public:
        void MakeUnique() const;
        uint32_t NumObjects() const { return m_objects.size(); }
        ResourceType GetObjectType(uint32_t objectIndex) const;
        std::string GetObjectName(uint32_t objectIndex) const { return m_objects.at(objectIndex).name; }
        int64_t GetOffsetFromName(const std::string& name, uint64_t offsetId) const;
        template<typename T>
        const T& GetObject(uint32_t objectIndex) const
        {
            return std::get<T>(m_objects.at(objectIndex).value);
        }
        template<typename T>
        void AppendObject(const std::string& name, const T& object)
        {
            m_objects.emplace_back(OrbObject{ name, object });
            m_objectIndices.emplace(name, static_cast<uint32_t>(m_objects.size() - 1));
        }
        template<typename T>
        void AppendObject(const std::string& name, T&& object)
        {
            m_objects.emplace_back(OrbObject{ name, std::forward<T>(object) });
            m_objectIndices.emplace(name, static_cast<uint32_t>(m_objects.size() - 1));
        }
    };

}