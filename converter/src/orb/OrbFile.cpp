#include "orb/OrbFile.hpp"
#include "orb/OrbIntermediate.hpp"
#include "implementation/misc/Logger.hpp"

#include <fstream>
#include <d3dcompiler.h>
#include <wrl/client.h>

#undef GetObject

#define MACRO_STRINGIFY(macro) #macro

namespace orbtool
{

    const char* ResourceTypeToString(ResourceType type)
    {
        switch(type)
        {
        case ResourceType::MATERIAL: return "Material"; 
        case ResourceType::MESH: return "Mesh";
        case ResourceType::TEXTURE:
            [[fallthrough]];
        case ResourceType::TEXTURE_REFERENCE: return "Texture";
        case ResourceType::INPUT_LAYOUT: return "Input Layout";
        case ResourceType::PIPELINE_STATE: return "Pipeline State";
        case ResourceType::SHADER_BINARY:
            [[fallthrough]];
        case ResourceType::SHADER_CODE: return "Shader";
        case ResourceType::SPLINE: return "Spline";
        case ResourceType::RASTERIZER_STATE: return "Rasterizer State";
        case ResourceType::BLEND_STATE: return "Blend State";
        case ResourceType::SAMPLER_STATE: return "Sampler State";

        case ResourceType::CUSTOM: return "Custom";
        default: return "Unknown"; 
        }
    }

    bool OrbFile::ParseFile(const fs::path& filepath)
    {
        m_filepath = filepath;
        std::ifstream file(filepath, std::ios::binary | std::ios::in);
        if (file.bad() || file.eof())
        {
            ORBIT_ERROR("Unable to open file '%s'.", filepath.generic_string().c_str());
            return false;
        }

        uint32_t numObjects  = 0;
        orbit::Version fileVersion = 0;

        file.read((char*)&fileVersion, sizeof(orbit::Version));
        if (sVersion < fileVersion) {
            ORBIT_ERROR("File version %d is more recent than parser version (%d) in %s", fileVersion.m_version, sVersion.m_version, filepath.generic_string().c_str());
            return false;
        }

        file.read((char*)&numObjects, sizeof(uint32_t));
        Index index;
        ResourceHeader header;
        std::string name;
        uint32_t nameLen;
        for (auto i = 0u; i < numObjects; ++i)
        {
            index.offset = file.tellg();
            file.read((char*)&header.id, sizeof(ResourceId));
            file.read((char*)&header.type, sizeof(ResourceType));
            file.read((char*)&header.payloadSize, sizeof(uint32_t));
            file.read((char*)&nameLen, sizeof(uint32_t));
            name.resize(nameLen);
            file.read(name.data(), nameLen);
            file.seekg(header.payloadSize, std::ios::cur);
            index.type = header.type;

            m_indices.emplace(header.id, index);

            if (file.bad())
                return true;
        }

        return true;
    }

    void OrbFile::PrintIndex() const
    {
        for (const auto& item : m_indices)
            printf_s("> %4lld: %s (%d)\n", item.first, ResourceTypeToString(item.second.type), static_cast<uint32_t>(item.second.type));
    }

    void OrbFile::PrintItemDetails(ResourceId itemId) const
    {
        auto it = m_indices.find(itemId);
        if (it == m_indices.end())
        {
            ORBIT_ERROR("Resource with ID '%lld' does not exist in this file.", itemId);
            return;
        }

        const auto& header = it->second;
        printf_s(">%4lld: %s\n", itemId, ResourceTypeToString(header.type));

        std::ifstream file(m_filepath, std::ios::binary | std::ios::in);
        if (file.bad() || file.eof())
        {
            ORBIT_ERROR("Unable to open file '%s'.", m_filepath.generic_string().c_str());
            return;
        }

        ResourceHeader lHeader;
        file.seekg(header.offset, std::ios::beg);
        file.read((char*)&lHeader, sizeof(ResourceHeader) - (sizeof(std::string) + 3));
        if (lHeader.id != itemId)
        {
            ORBIT_ERROR("Resource header ID does not match requested id: %lld != %lld", lHeader.id, itemId);
            return;
        }
        if (file.bad())
        {
            ORBIT_ERROR("Invalid resource file offset: %lld", header.offset);
            return;
        }

        std::string name;
        uint32_t nameLen;
        file.read((char*)&nameLen, sizeof(uint32_t));
        name.resize(nameLen);
        file.read(name.data(), nameLen);
        auto alloc = 30u;
        printf_s("  - %*s: %s\n", alloc, "Name", name.c_str());

        // Print resource details...
        switch (it->second.type)
        {
        case ResourceType::MATERIAL: {
            Vector4f
                diffuse,
                specular;
            float
                roughness;
            uint32_t flags = 0u;
            ResourceId
                colorId,
                normalId,
                roughnessId,
                occlusionId;
            file.read((char*)&diffuse, sizeof(Vector4f));
            file.read((char*)&specular, sizeof(Vector4f));
            file.read((char*)&roughness, sizeof(float));
            file.read((char*)&flags, sizeof(uint32_t));
            file.read((char*)&colorId, sizeof(ResourceId));
            file.read((char*)&normalId, sizeof(ResourceId));
            file.read((char*)&roughnessId, sizeof(ResourceId));
            file.read((char*)&occlusionId, sizeof(ResourceId));

            printf_s("  - %*s: %f, %f, %f, %f\n", alloc, "Diffuse Color", diffuse.x(), diffuse.y(), diffuse.z(), diffuse.w());
            printf_s("  - %*s: %f, %f, %f, %f\n", alloc, "Specular Color", specular.x(), specular.y(), specular.z(), specular.w());
            printf_s("  - %*s: 0x%x\n", alloc, "Flags", flags);
            printf_s("  - %*s: %f\n", alloc, "Roughness", roughness);
            printf_s("  - %*s: %lld\n", alloc, "Diffuse Texture", colorId);
            printf_s("  - %*s: %lld\n", alloc, "Normal Map", normalId);
            printf_s("  - %*s: %lld\n", alloc, "Roughness Map", roughnessId);
            printf_s("  - %*s: %lld\n", alloc, "Occlusion Map", occlusionId);
            break;
        }
        case ResourceType::MESH: {
            ResourceId materialId;
            uint64_t
                numIndices,
                numVertices;
            file.read((char*)&materialId, sizeof(ResourceId));
            file.read((char*)&numIndices, sizeof(uint64_t));
            file.read((char*)&numVertices, sizeof(uint64_t));

            printf_s("  - %*s: %lld\n", alloc, "Material", materialId + itemId);
            printf_s("  - %*s: %lld\n", alloc, "Number of vertices", numVertices);
            printf_s("  - %*s: %lld\n", alloc, "Number of indices", numIndices);
            break;
        }
        case ResourceType::INPUT_LAYOUT: {
            uint32_t numElements;
            file.read((char*)&numElements, sizeof(uint32_t));
            for (auto i = 0u; i < numElements; ++i) {
                uint32_t nameLen;
                std::string semantic_name;
                FormatType format;
                DataType datatype;
                uint32_t semantic_index = 0u;
                uint32_t slot = 0u;
                file.read((char*)&nameLen, sizeof(uint32_t));
                semantic_name.resize(nameLen);
                file.read(semantic_name.data(), nameLen);
                file.read((char*)&format, sizeof(FormatType));
                file.read((char*)&datatype, sizeof(DataType));
                file.read((char*)&semantic_index, sizeof(uint32_t));
                file.read((char*)&slot, sizeof(uint32_t));

                printf_s("  - %*s: { %10s%d, %s, %s, %d }\n", alloc, "", semantic_name.c_str(), semantic_index, orbit::FormatTypeToString(format), orbit::InputLayoutDataTypeToString(datatype), slot);
            }
            break;
        }
        case ResourceType::PIPELINE_STATE: {
            int64_t
                vs_offset = 0u,
                ps_offset = 0u,
                gs_offset = 0u,
                ds_offset = 0u,
                hs_offset = 0u,
                il_offset = 0u,
                rs_offset = 0u,
                bs_offset = 0u;
            file.read((char*)&vs_offset, sizeof(int64_t));
            file.read((char*)&ps_offset, sizeof(int64_t));
            file.read((char*)&gs_offset, sizeof(int64_t));
            file.read((char*)&ds_offset, sizeof(int64_t));
            file.read((char*)&hs_offset, sizeof(int64_t));
            file.read((char*)&il_offset, sizeof(int64_t));
            file.read((char*)&rs_offset, sizeof(int64_t));
            file.read((char*)&bs_offset, sizeof(int64_t));
            printf_s("  - %*s: %lld\n", alloc, "Vertex Shader", vs_offset);
            printf_s("  - %*s: %lld\n", alloc, "Pixel Shader", ps_offset);
            printf_s("  - %*s: %lld\n", alloc, "Geometry Shader", gs_offset);
            printf_s("  - %*s: %lld\n", alloc, "Domain Shader", ds_offset);
            printf_s("  - %*s: %lld\n", alloc, "Hull Shader", hs_offset);
            printf_s("  - %*s: %lld\n", alloc, "Input Layout", il_offset);
            printf_s("  - %*s: %lld\n", alloc, "Rasterizer State", rs_offset);
            printf_s("  - %*s: %lld\n", alloc, "Blend State", bs_offset);
            uint8_t numSampler = 0;
            file.read((char*)&numSampler, 1);
            for (auto i = 0u; i < numSampler; ++i)
            {
                uint32_t slot = 0;
                int64_t  sOffset = 0;

                file.read((char*)&slot, sizeof(uint32_t));
                file.read((char*)&sOffset, sizeof(int64_t));

                printf_s("  - %*s: %lld at slot %d\n", alloc, "Sampler", sOffset, slot);
            }
            break;
        }
        case ResourceType::SHADER_CODE:
        [[fallthrough]];
        case ResourceType::SHADER_BINARY: {
            ShaderType type;
            file.read((char*)&type, sizeof(ShaderType));
            printf_s("  - %*s: %s\n", alloc, "Shader Type", orbit::ShaderTypeToString(type));
            switch (type)
            {
            case ShaderType::SHADER_VERTEX:
            [[fallthrough]];
            case ShaderType::SHADER_PIXEL:
            [[fallthrough]];
            case ShaderType::SHADER_DOMAIN:
            [[fallthrough]];
            case ShaderType::SHADER_HULL:
            [[fallthrough]];
            case ShaderType::SHADER_GEOMETRY:
            [[fallthrough]];
            case ShaderType::SHADER_COMPUTE: {
                uint64_t codeLen = 0u;
                file.read((char*)&codeLen, sizeof(uint64_t));
                printf_s("  - %*s: %lld\n", alloc, "Code length", codeLen);
                break;
            }
            case ShaderType::SHADER_VERTEX_BINARY:
            [[fallthrough]];
            case ShaderType::SHADER_PIXEL_BINARY:
            [[fallthrough]];
            case ShaderType::SHADER_DOMAIN_BINARY:
            [[fallthrough]];
            case ShaderType::SHADER_HULL_BINARY:
            [[fallthrough]];
            case ShaderType::SHADER_GEOMETRY_BINARY:
            [[fallthrough]];
            case ShaderType::SHADER_COMPUTE_BINARY: {
                uint64_t binaryLen = 0u;
                file.read((char*)&binaryLen, sizeof(uint64_t));
                printf_s("  - %*s: %lld\n", alloc, "Bytecode length", binaryLen);
                break;
            }
            case ShaderType::SHADER_VERTEX_REFERENCE:
            [[fallthrough]];
            case ShaderType::SHADER_PIXEL_REFERENCE:
            [[fallthrough]];
            case ShaderType::SHADER_DOMAIN_REFERENCE:
            [[fallthrough]];
            case ShaderType::SHADER_HULL_REFERENCE:
            [[fallthrough]];
            case ShaderType::SHADER_GEOMETRY_REFERENCE:
            [[fallthrough]];
            case ShaderType::SHADER_COMPUTE_REFERENCE:{
                uint32_t pathLen = 0u;
                file.read((char*)&pathLen, sizeof(uint64_t));
                std::string path;
                path.resize(pathLen);
                file.read(path.data(), pathLen);
                printf_s("  - %*s: %s\n", alloc, "Reference path", path.c_str());
                break;
            }
            }
            break;
        }
        case ResourceType::SPLINE: {

            break;
        }
        case ResourceType::TEXTURE: {
            uint64_t binaryLen = 0u;
            file.read((char*)&binaryLen, sizeof(uint64_t));
            printf_s("  - %*s: %lld\n", alloc, "Texture size", binaryLen);
            break;
        }
        case ResourceType::TEXTURE_REFERENCE: {
            uint32_t pathLen = 0u;
            file.read((char*)&pathLen, sizeof(uint32_t));
            std::string path;
            path.resize(pathLen);
            file.read(path.data(), pathLen);
            printf_s("  - %*s: %s\n", alloc, "Texture path", path.c_str());
            break;
        }
        case ResourceType::BLEND_STATE: {
            bool 
                alphaToCoverage,
                blendEnabled;
            file.read((char*)&alphaToCoverage, 1);
            file.read((char*)&blendEnabled, 1);
            printf_s("  - %*s: %s\n", alloc, "AlphaToCoverage", alphaToCoverage ? "true" : "false");
            printf_s("  - %*s: %s\n", alloc, "Blend Enabled", blendEnabled ? "true" : "false");
            if (blendEnabled)
            {
                /*
                output.write((const char*)&state.alphaToCoverageEnabled, 1);
                output.write((const char*)&state.blendEnabled, 1);
                if (state.blendEnabled)
                {
                    output.write((const char*)state.blendFactor, 4 * sizeof(float));
                    output.write((const char*)&state.channelMask, 1);
                */
                float factor[4];
                EBlendOp
                    blendOp,
                    alphaBlendOp;
                EBlend
                    src,
                    srcAlpha,
                    dest,
                    destAlpha;
                uint8_t channels;
                file.read((char*)factor, 4 * sizeof(float));
                file.read((char*)&channels, 1);
                file.read((char*)&blendOp, 1);
                file.read((char*)&alphaBlendOp, 1);
                file.read((char*)&src, 1);
                file.read((char*)&srcAlpha, 1);
                file.read((char*)&dest, 1);
                file.read((char*)&destAlpha, 1);
                printf_s("  - %*s: %f, %f, %f, %f\n", alloc, "BlendFactor", factor[0],factor[1],factor[2],factor[3]);
                printf_s("  - %*s: %s\n", alloc, "BlendOp", orbit::EBlendOperationToString(blendOp));
                printf_s("  - %*s: %s\n", alloc, "AlphaBlendOp", orbit::EBlendOperationToString(alphaBlendOp));
                printf_s("  - %*s: %s\n", alloc, "SourceBlend", orbit::EBlendToString(src));
                printf_s("  - %*s: %s\n", alloc, "SourceAlphaBlend", orbit::EBlendToString(srcAlpha));
                printf_s("  - %*s: %s\n", alloc, "DestBlend", orbit::EBlendToString(dest));
                printf_s("  - %*s: %s\n", alloc, "DestAlphaBlend", orbit::EBlendToString(destAlpha));
                printf_s("  - %*s: ", alloc, "Channels");
                if ((channels & 1) == 1)
                    printf_s("RED, ");
                if ((channels & 2) == 2)
                    printf_s("GREEN, ");
                if ((channels & 4) == 4)
                    printf_s("BLUE, ");
                if ((channels & 8) == 8)
                    printf_s("ALPHA");
                printf_s("\n");
            }
            break;
        }
        case ResourceType::SAMPLER_STATE: {
            EFilter filter;
            EAddress 
                addressx1,
                addressx2,
                addressx3;
            file.read((char*)&filter, 1);
            file.read((char*)&addressx1, 1);
            file.read((char*)&addressx2, 1);
            file.read((char*)&addressx3, 1);
            printf_s("  - %*s: %s\n", alloc, "Filter", orbit::ESamplerFilterToString(filter));
            printf_s("  - %*s: %s\n", alloc, "Address X0", orbit::ETextureAddressToString(addressx1));
            printf_s("  - %*s: %s\n", alloc, "Address X1", orbit::ETextureAddressToString(addressx2));
            printf_s("  - %*s: %s\n", alloc, "Address X2", orbit::ETextureAddressToString(addressx3));
            break;
        }
        case ResourceType::RASTERIZER_STATE: {
            CullMode cullmode;
            FillMode fillmode;
            file.read((char*)&cullmode, sizeof(CullMode));
            file.read((char*)&fillmode, sizeof(FillMode));
            printf_s("  - %*s: %s\n", alloc, "Cull Mode", orbit::CullModeToString(cullmode));
            printf_s("  - %*s: %s\n", alloc, "Fill Mode", orbit::FillModeToString(fillmode));
            break;
        }
        }
    }

    void OrbFile::UpdateFile(const fs::path& target) const
    {

    }

    void OrbFile::WriteIntermediate(const OrbIntermediate& orb, const fs::path& target) const
    {
        if (target.empty() && !m_filepath.empty())
            return WriteIntermediate(orb, m_filepath);

        auto outputExists = fs::exists(target);
        if (!outputExists)
        {
            // Create file so that it can be opened in read/write mode
            std::ofstream file(target, std::ios::binary | std::ios::out);
        }
        
        std::fstream output(target, std::ios::binary | std::ios::in | std::ios::out | std::ios::beg);
        output.seekg(0, std::ios::beg);
        if (!output.is_open())
        {
            ORBIT_ERROR("Failed to open file '%s'", target.generic_string().c_str());
            return;
        }

        uint32_t numObjects = m_indices.size() + orb.NumObjects();
        if (outputExists)
        {
            orbit::Version fileVersion = 0;
            output.read((char*)&fileVersion, sizeof(orbit::Version));
            if (sVersion < fileVersion) {
                ORBIT_ERROR("File version %d is more recent than parser version (%d) in '%s'. Update your parser first.", fileVersion.m_version, sVersion.m_version, target.generic_string().c_str());
                return;
            }
            else if (sVersion > fileVersion) {
                ORBIT_ERROR("File version %d is deprecated '%s'. Update the file first with 'orbtool -input %s -update'.", fileVersion.m_version, target.generic_string().c_str(), target.generic_string().c_str());
                return;
            }
        }
        else
            output.write((const char*)&sVersion, sizeof(orbit::Version));
        
        output.write((const char*)&numObjects, sizeof(uint32_t));
        output.seekg(0, std::ios::end);

        uint64_t id = NextIndex();
        auto start_id = id;
        uint32_t dummy = 0u;
        auto objectsToBeWritten = orb.NumObjects();
        for (auto i = 0u; i < objectsToBeWritten; ++i)
        {
            ResourceType type = orb.GetObjectType(i);
            output.write((const char*)&id, sizeof(ResourceId));
            output.write((const char*)&type, sizeof(ResourceType));
            auto prevPos = output.tellg();
            output.write((const char*)&dummy, sizeof(uint32_t));
            auto name = orb.GetObjectName(i);
            uint32_t nameLen = name.length();
            output.write((const char*)&nameLen, sizeof(uint32_t));
            output.write(name.data(), nameLen);
            switch (type)
            {
            case ResourceType::MATERIAL: {
                const auto& material = orb.GetObject<OrbMaterial>(i);
                ResourceId dmId = orb.GetOffsetFromName(material.diffuseTextureId, i);
                ResourceId nmId = orb.GetOffsetFromName(material.normalMapId, i);
                ResourceId rmId = orb.GetOffsetFromName(material.roughnessMapId, i);
                ResourceId omId = orb.GetOffsetFromName(material.occlusionMapId, i);
                uint32_t flags = 0u;
                if (dmId != 0)
                    flags |= static_cast<uint32_t>(MaterialFlag::FLAG_HAS_ALBEDO_TEXTURE);
                if (nmId != 0)
                    flags |= static_cast<uint32_t>(MaterialFlag::FLAG_HAS_NORMAL_MAP);
                if (rmId != 0)
                    flags |= static_cast<uint32_t>(MaterialFlag::FLAG_HAS_ROUGHNESS_TEXTURE);
                if (omId != 0)
                    flags |= static_cast<uint32_t>(MaterialFlag::FLAG_HAS_OCCLUSION_MAP);
                
                output.write((const char*)&material.diffuse, sizeof(Vector4f));
                output.write((const char*)&material.specular, sizeof(Vector4f));
                output.write((const char*)&material.roughness, sizeof(float));
                output.write((const char*)&flags, sizeof(uint32_t));
                output.write((const char*)&dmId, sizeof(ResourceId));
                output.write((const char*)&nmId, sizeof(ResourceId));
                output.write((const char*)&rmId, sizeof(ResourceId));
                output.write((const char*)&omId, sizeof(ResourceId));
            }
                break;
            case ResourceType::MESH: {
                const auto& mesh = orb.GetObject<OrbMesh>(i);
                int64_t materialIdOffset = orb.GetOffsetFromName(mesh.material, i);
                output.write((const char*)&materialIdOffset, sizeof(int64_t));
                uint64_t numIndices = mesh.indices.size();
                uint64_t numVertices = mesh.vertices.size();
                output.write((const char*)&numIndices, sizeof(uint64_t));
                output.write((const char*)&numVertices, sizeof(uint64_t));
                output.write((const char*)mesh.indices.data(), sizeof(uint32_t) * numIndices);
                output.write((const char*)mesh.vertices.data(), sizeof(OrbVertex) * numVertices);
            }
                break;
            case ResourceType::INPUT_LAYOUT: {
                const auto& layout = orb.GetObject<OrbInputLayout>(i);
                uint32_t numElements = layout.elements.size();
                output.write((const char*)&numElements, sizeof(uint32_t));
                for (const auto& element : layout.elements)
                {
                    uint32_t nameLen = element.semantic_name.length();
                    output.write((const char*)&nameLen, sizeof(uint32_t));
                    output.write(element.semantic_name.data(), nameLen);
                    output.write((const char*)&element.format, sizeof(FormatType));
                    output.write((const char*)&element.type, sizeof(DataType));
                    output.write((const char*)&element.semantic_index, sizeof(uint32_t));
                    output.write((const char*)&element.inputSlot, sizeof(uint32_t));
                }
            }
                break;
            case ResourceType::PIPELINE_STATE: {
                const auto& state = orb.GetObject<OrbPipelineState>(i);
                auto vsId = orb.GetOffsetFromName(state.vShaderId, i);
                auto psId = orb.GetOffsetFromName(state.pShaderId, i);
                auto gsId = orb.GetOffsetFromName(state.gShaderId, i);
                auto dsId = orb.GetOffsetFromName(state.dShaderId, i);
                auto hsId = orb.GetOffsetFromName(state.hShaderId, i);
                auto ilId = orb.GetOffsetFromName(state.iLayoutId, i);
                auto rsId = orb.GetOffsetFromName(state.rsStateId, i);
                auto bsId = orb.GetOffsetFromName(state.bsStateId, i);
                output.write((const char*)&vsId, sizeof(int64_t));
                output.write((const char*)&psId, sizeof(int64_t));
                output.write((const char*)&gsId, sizeof(int64_t));
                output.write((const char*)&dsId, sizeof(int64_t));
                output.write((const char*)&hsId, sizeof(int64_t));
                output.write((const char*)&ilId, sizeof(int64_t));
                output.write((const char*)&rsId, sizeof(int64_t));
                output.write((const char*)&bsId, sizeof(int64_t));
                uint8_t size = static_cast<uint8_t>(state.sStateIds.size());
                output.write((const char*)&size, 1);
                for (const auto&[slot, sampler] : state.sStateIds)
                {
                    auto samplerId = orb.GetOffsetFromName(sampler, i);
                    output.write((const char*)&slot, sizeof(uint32_t));
                    output.write((const char*)&samplerId, sizeof(int64_t));
                }
            }
                break;
            case ResourceType::SHADER_BINARY: {
                const auto& shader = orb.GetObject<OrbShaderBinary>(i);
                output.write((const char*)&shader.type, sizeof(ShaderType));
                if (shader.reference_path.empty())
                {
                    // not a reference
                    Microsoft::WRL::ComPtr<ID3DBlob> binary;
                    D3DReadFileToBlob(shader.reference_path.c_str(), binary.ReleaseAndGetAddressOf());
                    auto fileLen = binary->GetBufferSize();
                    output.write((const char*)&fileLen, sizeof(uint64_t));
                    output.write((const char*)binary->GetBufferPointer(), fileLen);
                }
                else
                {
                    auto pathLen = shader.reference_path.generic_string().length();
                    output.write((const char*)&pathLen, sizeof(uint64_t));
                    output.write(shader.reference_path.generic_string().data(), pathLen);
                }
            }
                break;
            case ResourceType::SHADER_CODE: {
                const auto& shader = orb.GetObject<OrbShaderCode>(i);
                output.write((const char*)&shader.type, sizeof(ShaderType));
                if (shader.compile)
                {
                    if (shader.shader_model != "glsl")
                    {
                        std::string macros;
                        for (const auto& macro : shader.compiler_macros)
                            macros += "/D" + macro + " ";
                        macros += "/D" ORBIT_RENDER_ENGINE " ";
                        char command[1000];
                        auto target = shader.shader_code;
                        target.replace_extension(".fxc-tmp");
                        auto count = sprintf_s(
                            command, 
                            "fxc /E %s /T %s /Od /Zi /Fo %s %s %s >> fxc_log.txt 2>&1", 
                            shader.entry_point.c_str(),
                            shader.shader_model.c_str(),
                            target.generic_string().c_str(),
                            shader.shader_code.generic_string().c_str(),
                            macros.c_str()
                        );
                        if (fs::exists("fxc_log.txt"))
                            fs::remove("fxc_log.txt");
                        ORBIT_LOG("Compiling file: %s", shader.shader_code.generic_string().c_str());
                        auto result = system(command);
                        if (result != 0)
                            ORBIT_THROW("Compilation failed. For more details look at 'fxc_log.txt'");
                        
                        Microsoft::WRL::ComPtr<ID3DBlob> binary;
                        D3DReadFileToBlob(target.c_str(), binary.ReleaseAndGetAddressOf());
                        auto fileLen = binary->GetBufferSize();
                        output.write((const char*)&fileLen, sizeof(uint64_t));
                        output.write((const char*)binary->GetBufferPointer(), fileLen);
                        fs::remove(target);
                    }
                    else 
                    {
                        // glslang compilation to be done
                        ORBIT_THROW("GLSL compilation not supported yet");
                    }
                }
                else
                {
                    uint32_t len = shader.shader_model.length();
                    output.write((const char*)&len, sizeof(uint32_t));
                    output.write(shader.shader_model.data(), len);
                    if (shader.shader_model != "glsl")
                    {
                        len = shader.entry_point.length();
                        output.write((const char*)&len, sizeof(uint32_t));
                        output.write(shader.entry_point.data(), len);
                    }

                    if (shader.reference_path.empty())
                    {
                        // not a reference
                        Microsoft::WRL::ComPtr<ID3DBlob> binary;
                        D3DReadFileToBlob(shader.shader_code.c_str(), binary.ReleaseAndGetAddressOf());
                        auto fileLen = binary->GetBufferSize();
                        output.write((const char*)&fileLen, sizeof(uint64_t));
                        output.write((const char*)binary->GetBufferPointer(), fileLen);
                    }
                    else
                    {
                        auto path = shader.reference_path.generic_string();
                        uint32_t pathLen = path.length();

                        output.write((const char*)&pathLen, sizeof(uint32_t));
                        output.write(path.data(), pathLen);
                    }
                }
            }
                break;
            case ResourceType::RASTERIZER_STATE: {
                const auto& rState = orb.GetObject<OrbRasterizerState>(i);
                output.write((char*)&rState.cull_mode, sizeof(CullMode));
                output.write((char*)&rState.fill_mode, sizeof(FillMode));
                break;
            }
            case ResourceType::TEXTURE: {
                std::ifstream file(orb.GetObject<OrbTexture>(i).texturePath, std::ios::binary | std::ios::in);
                if (!file.is_open())
                    ORBIT_THROW("Cannot open file '%s'", orb.GetObject<OrbTexture>(i).texturePath.generic_string().c_str());

                file.seekg(0, std::ios::end);
                size_t fileLen = file.tellg();
                file.seekg(0, std::ios::beg);

                std::string str;
                str.resize(fileLen);
                file.read(str.data(), fileLen);
                output.write((const char*)&fileLen, sizeof(uint64_t));
                output.write(str.data(), fileLen);
            }
                break;
            case ResourceType::TEXTURE_REFERENCE: {
                const auto& path = orb.GetObject<OrbTexture>(i).referencePath.generic_string();
                uint32_t pathLen = path.length();
                output.write((const char*)&pathLen, sizeof(uint32_t));
                output.write(path.data(), pathLen);
            }
                break;
            case ResourceType::BLEND_STATE: {
                const auto& state = orb.GetObject<OrbBlendState>(i);
                output.write((const char*)&state.alphaToCoverageEnabled, 1);
                output.write((const char*)&state.blendEnabled, 1);
                if (state.blendEnabled)
                {
                    output.write((const char*)state.blendFactor, 4 * sizeof(float));
                    output.write((const char*)&state.channelMask, 1);
                    output.write((const char*)&state.blendOperation, 1);
                    output.write((const char*)&state.alphaBlendOperation, 1);
                    output.write((const char*)&state.srcBlend, 1);
                    output.write((const char*)&state.srcAlphaBlend, 1);
                    output.write((const char*)&state.destBlend, 1);
                    output.write((const char*)&state.destAlphaBlend, 1);
                }
                break;
            }
            case ResourceType::SAMPLER_STATE: {
                const auto& state = orb.GetObject<OrbSamplerState>(i);
                output.write((const char*)&state.filter, 1);
                output.write((const char*)&state.addressX1, 1);
                output.write((const char*)&state.addressX2, 1);
                output.write((const char*)&state.addressX3, 1);
                break;
            }
            default:
                ORBIT_THROW("Cannot serialize object type %s", ResourceTypeToString(type));
                break;
            }
            auto afterPos = output.tellg();
            uint32_t payloadSize = (afterPos - prevPos) - (sizeof(uint32_t) * 2 + nameLen);
            output.seekg(prevPos, std::ios::beg);
            output.write((const char*)&payloadSize, sizeof(uint32_t));
            output.seekg(0, std::ios::end);
            ++id;
        }
    }

    uint64_t OrbFile::NextIndex() const
    {
        uint64_t max = 0u;
        for (const auto& index : m_indices)
        {
            if (index.first > max)
                max = index.first;
        }
        return max + 1;
    }

}

#undef MACRO_STRINGIFY