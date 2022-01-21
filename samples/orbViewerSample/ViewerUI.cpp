#include "ViewerUI.hpp"
#include "implementation/engine/Engine.hpp"

#undef new
#include <imgui.h>

#ifdef ORBIT_WINDOWS
#include <Windows.h>
#endif

void ViewerUI::OpenFileDialogue()
{
    char filepath[MAX_PATH];
    filepath[0] = '\0';
#ifdef ORBIT_WINDOWS
    OPENFILENAMEA filedialog;
    ZeroMemory(&filedialog, sizeof(OPENFILENAMEA));
    filedialog.lStructSize = sizeof(OPENFILENAMEA);
    filedialog.lpstrFilter = "Orb Files\0*.orb\0\0";
    filedialog.lpstrFile = filepath;
    filedialog.hwndOwner = ENGINE->Window()->GetHandle();
    filedialog.nMaxFile = MAX_PATH;
    
    auto cwd = std::filesystem::current_path();
    GetOpenFileNameA(&filedialog);
    std::filesystem::current_path(cwd);
#endif
    ENGINE->RMParseFile(filepath);
    m_parser.ParseFile(filepath);
}

void ViewerUI::Init()
{
    m_propertyLoaded = true;
}

void ViewerUI::Update(const orbit::Time& dt)
{
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Load...")) { 
                OpenFileDialogue();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    
    ImGuiWindowFlags window_flags = 
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoDecoration | 
        ImGuiWindowFlags_NoSavedSettings | 
        ImGuiWindowFlags_NoFocusOnAppearing | 
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoCollapse;
    ImGui::SetNextWindowPos(ImVec2{ 0.f, 20.f }, ImGuiCond_Always);
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ io.DisplaySize.x  / 3.f, 0.75f * io.DisplaySize.y },
        ImVec2{ io.DisplaySize.x / 2.f, 0.75f * io.DisplaySize.y }
    );
    ImGui::SetNextWindowBgAlpha(1.f);
    const auto& entities = m_parser.GetEntities();
    if (ImGui::Begin("Entities", nullptr, window_flags))
    {
        ImGui::BeginChild("EntityContainer");
        if (ImGui::TreeNode("Geometries"))
        {
            for (const auto& header : entities)
            {
                if (header.type == orbit::ResourceType::MESH)
                    LoadGeometry(header.name);
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Materials"))
        {
            for (const auto& header : entities)
            {
                if (header.type == orbit::ResourceType::MATERIAL)
                    LoadMaterial(header.name);
            }

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Textures"))
        {
            for (const auto& header : entities)
            {
                if (header.type == orbit::ResourceType::TEXTURE || 
                    header.type == orbit::ResourceType::TEXTURE_REFERENCE)
                    LoadTexture(header.name);
            }

            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Lights"))
        {
            
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Misc"))
        {
            for (const auto& header : entities)
            {
                if (header.type != orbit::ResourceType::TEXTURE &&
                    header.type != orbit::ResourceType::TEXTURE_REFERENCE &&
                    header.type != orbit::ResourceType::MATERIAL && 
                    header.type != orbit::ResourceType::MESH)
                    LoadMisc(header.name);
            }
            
            ImGui::TreePop();
        }
        ImGui::EndChild();
        ImGui::End();
    }
    ImGui::SetNextWindowPos(ImVec2{ 0.f, 0.75f * io.DisplaySize.y + 20.f }, ImGuiCond_Always);
    ImGui::SetNextWindowSizeConstraints(
        ImVec2{ io.DisplaySize.x, 0.25f * io.DisplaySize.y - 20.f },
        ImVec2{ io.DisplaySize.x, 0.25f * io.DisplaySize.y - 20.f }
    );
    if (ImGui::Begin("Properties", nullptr, window_flags))
    {
        for (const auto& header : entities)
        {
            if (header.name == m_property)
            {
                ImGui::Text("%s : %s", m_property.c_str(), orbit::ResourceTypeToString(header.type));
                PropertyView(header);
                break;
            }
        }
        ImGui::End();
    }
}

void ViewerUI::PropertyView(const orbit::ResourceHeader& header)
{

}

void ViewerUI::LoadGeometry(const std::string& id)
{
    if (m_loaded == id)
        ImGui::Text(id.c_str());
    else
    {
        if (ImGui::Button(id.c_str()))
        {
            m_loaded = id;
            auto view = ENGINE->GetCurrentScene()->FindObjectT<O3DView>("3DView");
            view->LoadGeometry(id);
        }
    }
}

void ViewerUI::LoadMaterial(const std::string& id)
{
    if (m_loaded == id)
        ImGui::Text(id.c_str());
    else
    {
        if (ImGui::Button(id.c_str()))
        {
            m_loaded = id;
            auto view = ENGINE->GetCurrentScene()->FindObjectT<O3DView>("3DView");
            view->LoadMaterial(id);
        }
    }
}

void ViewerUI::LoadTexture(const std::string& id)
{
    if (m_loaded == id)
        ImGui::Text(id.c_str());
    else
    {
        if (ImGui::Button(id.c_str()))
        {
            m_loaded = id;
            auto view = ENGINE->GetCurrentScene()->FindObjectT<O3DView>("3DView");
            view->LoadTexture(id);
        }
    }
}

void ViewerUI::LoadMisc(const std::string& id)
{
    if (m_property == id)
        ImGui::Text(id.c_str());
    else
    {
        if (ImGui::Button(id.c_str()))
        {
            m_propertyLoaded = false;
            m_property = id;
        }
    }
}
