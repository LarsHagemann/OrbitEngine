#include "implementation/engine/ResourceManager.hpp"
#include "implementation/misc/Logger.hpp"
#include "interfaces/rendering/Material.hpp"

#include <fstream>

#undef new

#include <imgui.h>

namespace orbit
{

    void ResourceManager::RMInit()
    {
        if (!RMParseFile("DefaultResources.orb"))
            ORBIT_THROW("Unable to load default resources. Make sure the file 'DefaultResources.orb' can be found by orbit!");

        auto make_resource = [&](const char* name, auto ptr) {
            auto id = RMGetIdFromName(name);
            ptr->SetId(id);
            m_resources.emplace(id, ptr);
            return ptr;
        };
        
        make_resource("pipeline_states/default", std::make_shared<PipelineState>());
        make_resource("materials/default", std::make_shared<MaterialBase>());
        make_resource("shader/vertex/default", std::make_shared<VertexShader>());
        make_resource("shader/pixel/default", std::make_shared<PixelShader>());
        make_resource("input_layouts/default", std::make_shared<InputLayout>());
    }

    bool ResourceManager::RMParseFile(const fs::path& path)
    {
        ORBIT_INFO_LEVEL(ORBIT_LEVEL_DEBUG, "Loading file '%s'", path.generic_string().c_str());
        std::ifstream file(path, std::ios::binary | std::ios::in);
        if (!file.is_open() || file.eof())
            return false;

        uint32_t numObjects  = 0;
        Version fileVersion = 0;

        file.read((char*)&fileVersion, sizeof(Version));
        if (sVersion < fileVersion) {
            ORBIT_ERROR("File version %d is more recent than parser version (%d) in %s", fileVersion.m_version, sVersion.m_version, path.generic_string().c_str());
            return false;
        }

        file.read((char*)&numObjects, sizeof(uint32_t));
        Index index;
        ResourceHeader header;
        m_parsedFiles.emplace_back(path);
        index.fileIndex = m_parsedFiles.size() - 1;
        uint32_t nameLen = 0u;
        for (auto i = 0u; i < numObjects; ++i)
        {
            nameLen = 0u;
            file.read((char*)&header.id, sizeof(ResourceId));
            file.read((char*)&header.type, sizeof(ResourceType));
            file.read((char*)&header.payloadSize, sizeof(uint32_t));
            file.read((char*)&nameLen, sizeof(uint32_t));
            header.name.resize(nameLen);
            file.read(header.name.data(), nameLen);
            index.offset = file.tellg();
            file.seekg(header.payloadSize, std::ios::cur);
            index.type = header.type;
            
            auto id = RMGetNextResourceId();
            RMRegisterResourceName(header.name, id);
            m_index.emplace(id, index);

            if (file.bad())
                return false;
        }

        return true;
    }

    bool ResourceManager::RMGetStream(ResourceId id, std::ifstream* stream) const
    {
        auto headerIt = m_index.find(id);
        if (headerIt == m_index.end())
        {
            ORBIT_ERROR("Unable to load resource %lld", id);
            return false;
        }
        const auto& filepath = m_parsedFiles.at(headerIt->second.fileIndex);
        stream->open(filepath, std::ios::binary | std::ios::in);
        stream->seekg(headerIt->second.offset, std::ios::beg);
        return stream->good();
    }

    bool ResourceManager::RMRegisterResourceName(const std::string& name, ResourceId id)
    {
#ifdef _DEBUG
        auto it = m_resourceNames.find(name);
        if (it != m_resourceNames.end())
        {
            if (it->second == id)
                return true; // object already registered
            
            ORBIT_ERROR("Resource with name '%s' does already exist with id %lld (while trying to assign id %lld)", name.c_str(), it->second, id);
            return false;
        }
#endif
        m_resourceNames.emplace(name, id);
        return true;
    }

    ResourceId ResourceManager::RMGetIdFromName(const std::string& name) const
    {
        auto it = m_resourceNames.find(name);
        if (it == m_resourceNames.end())
        {
#ifdef _DEBUG
            ORBIT_ERROR("Unable to find object with identifier '%s'", name.c_str());
#endif
            return std::numeric_limits<ResourceId>::max();
        }

        return it->second;
    }

    ResourceType ResourceManager::RMGetResourceType(ResourceId id) const
    {
        auto it = m_index.find(id);
        if (it == m_index.end())
            return ResourceType::UNDEFINED;

        return it->second.type;
    }

    void ResourceManager::RMDrawDebug() const
    {
        ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoDecoration | 
			ImGuiWindowFlags_NoSavedSettings | 
			ImGuiWindowFlags_NoFocusOnAppearing | 
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowPos(ImVec2{ 0.f, 0.f }, ImGuiCond_Always);
        ImGui::SetNextWindowSizeConstraints(
            ImVec2{ io.DisplaySize.x  / 3.f, io.DisplaySize.y / 2.f},
            ImVec2{ io.DisplaySize.x / 2.f, io.DisplaySize.y }
        );
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::Begin("Resources", nullptr, window_flags);
        for (const auto& resource : m_resourceNames)
        {
            if (ImGui::TreeNode(resource.first.c_str()))
            {
                const auto& index = m_index.find(resource.second);
                bool t = false;
                auto it = m_resources.find(resource.second);
                if (it != m_resources.end())
                    t = it->second->IsLoaded();
                
                ImGui::Checkbox("Loaded: ", &t);
                ImGui::TreePop();
            }
        }
        if (ImGui::TreeNode("Camera"))
        {
            if (ImGui::TreeNode("Projection matrix"))
            {
                const auto m = ENGINE->GetCurrentScene()->GetCamera()->GetProjectionMatrix();
                ImGui::Text("%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
                    m(0, 0), m(0, 1), m(0, 2), m(0, 3),
                    m(1, 0), m(1, 1), m(1, 2), m(1, 3),
                    m(2, 0), m(2, 1), m(2, 2), m(2, 3),
                    m(3, 0), m(3, 1), m(3, 2), m(3, 3)
                );
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        ImGui::End();
    }

}