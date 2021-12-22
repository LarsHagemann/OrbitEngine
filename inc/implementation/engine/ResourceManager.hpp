#pragma once
#include "implementation/Common.hpp"
#include "implementation/misc/Version.hpp"
#include "implementation/engine/ResourceType.hpp"
#include "implementation/misc/ResourceHeader.hpp"

#include "implementation/backends/impl/PipelineStateImpl.hpp"
#include "implementation/backends/impl/PixelShaderImpl.hpp"
#include "implementation/backends/impl/VertexShaderImpl.hpp"
#include "implementation/backends/impl/GeometryShaderImpl.hpp"
#include "implementation/backends/impl/DomainShaderImpl.hpp"
#include "implementation/backends/impl/HullShaderImpl.hpp"
#include "implementation/backends/impl/TextureImpl.hpp"
#include "implementation/backends/impl/InputLayoutImpl.hpp"
#include "implementation/backends/impl/PipelineStateImpl.hpp"

#include "interfaces/misc/UnLoadable.hpp"

#include <istream>
#include <string>
#include <unordered_map>

namespace orbit
{

    class MaterialBase;
    class Mesh;

    class ResourceManager 
    {
    protected:
        struct Index
        {
            size_t       fileIndex;
            size_t       offset;
            ResourceType type;
        };
        std::vector<fs::path> m_parsedFiles;
        std::unordered_map<ResourceId, Index> m_index;
        std::unordered_map<std::string, ResourceId> m_resourceNames;
        std::unordered_map<ResourceId, SPtr<UnLoadable>> m_resources;
        static constexpr Version sVersion = Version{ 0, 0, 1 };
        mutable ResourceId m_currentId = 1u;
    protected:
        void                  RMInit();
    public:
        ResourceId            RMGetNextResourceId() const { return m_currentId++; }
        ResourceId            RMGetIdFromName(const std::string& name) const;
        bool                  RMParseFile(const fs::path& path);
        bool                  RMGetStream(ResourceId id, std::ifstream* stream) const;
        bool                  RMRegisterResourceName(const std::string& name, ResourceId id);
        ResourceType          RMGetResourceType(ResourceId id) const;
        template<typename ResourceType>
        SPtr<ResourceType> RMLoadResource(ResourceId id)
        {
            auto rIt = m_resources.find(id);
            if (rIt == m_resources.end()) {
                auto it = m_index.find(id);
                if (it == m_index.end()) {
                    ORBIT_ERROR("Failed to load Resource %lld", id);
                    if constexpr (std::is_same_v<ResourceType, VertexShader>)
                        return RMLoadResource<ResourceType>(RMGetIdFromName("shader/vertex/default"));
                    if constexpr (std::is_same_v<ResourceType, PixelShader>)
                        return RMLoadResource<ResourceType>(RMGetIdFromName("shader/pixel/default"));
                    if constexpr (std::is_same_v<ResourceType, Texture>)
                        return RMLoadResource<ResourceType>(RMGetIdFromName("textures/default"));
                    if constexpr (std::is_same_v<ResourceType, InputLayout>)
                        return RMLoadResource<ResourceType>(RMGetIdFromName("input_layouts/default"));
                    if constexpr (std::is_same_v<ResourceType, MaterialBase>)
                        return RMLoadResource<ResourceType>(RMGetIdFromName("materials/default_debug"));
                    if constexpr (std::is_same_v<ResourceType, PipelineState>)
                        return RMLoadResource<ResourceType>(RMGetIdFromName("pipeline_states/default"));

                    return nullptr;
                }
                auto resource = std::static_pointer_cast<UnLoadable>(
                    std::make_shared<ResourceType>());
                resource->SetId(id);
                if (!resource->Load())
                    return nullptr;
                
                m_resources.emplace(id, resource);
                return std::static_pointer_cast<ResourceType>(resource);
            }
            if (!rIt->second->IsLoaded())
                rIt->second->Load();
            return std::static_pointer_cast<ResourceType>(rIt->second);
        }
        void RMDrawDebug() const;

        static constexpr auto RMInvalidId = std::numeric_limits<ResourceId>::max();
    };

}
