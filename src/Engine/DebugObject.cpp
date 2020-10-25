#include "Engine/DebugObject.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Scene.hpp"

#include "Engine/Component/EventDriven.hpp"
#include "Engine/Component/Renderable.hpp"

#include "imgui.h"

namespace orbit
{

    void DebugObject::Init()
    {
        Object::Init();
        _kHandler = AddComponent<KeyboardComponent>("keyboard_handler");
    }

    void DebugObject::End()
    {
        ImGui::End();
    }

    void DebugObject::ShowDebug()
    {
        ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = 
			ImGuiWindowFlags_NoDecoration | 
			ImGuiWindowFlags_NoSavedSettings | 
			ImGuiWindowFlags_NoFocusOnAppearing | 
			ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoMove;
		ImGui::SetNextWindowPos(ImVec2{ 0.f, 0.f }, ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2{ io.DisplaySize.x  / 3.f, io.DisplaySize.y }, ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.35f);
		ImGui::Begin("Debug", &_displayDebug, window_flags);

        auto scene = Engine::Get()->_scene;

        if (ImGui::TreeNode(FormatString("Objects in the current Scene: %d", scene->_objects.size()).c_str()))
        {
            for (auto object : scene->_objects)
            {
                if (ImGui::TreeNode(object.first.c_str()))
                {
                    for (auto component : object.second->_components)
                    {
                        if (ImGui::TreeNode(component.first.c_str()))
                        {                            
                            component.second->DebugGUI(std::static_pointer_cast<DebugObject>(shared_from_this()));
                            ImGui::TreePop();
                        }
                    }
                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Camera"))
        {
            auto camera = scene->GetCamera();
            if (ImGui::TreeNode("Position/Rotation"))
            {
                ShowVector(camera->GetTransform()->GetCombinedTranslation());
                ShowQuaternion(camera->GetTransform()->GetCombinedRotation());

                float fov = Math<float>::PIDIV4;
                auto& io = ImGui::GetIO();
                if (ImGui::SliderFloat("Vertical FOV", &fov, 0.f, Math<float>::_2PI, "FOV: %f", 1.f))
                {
                    camera->RecalculateProjectionMatrix(fov, io.DisplaySize.x / io.DisplaySize.y, 0.5f, 100.f);
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("View Matrix"))
            {
                ShowMatrix(camera->GetViewMatrix());
                ImGui::TreePop();
            }
            if (ImGui::TreeNode("Projection Matrix"))
            {
                ShowMatrix(camera->GetProjectionMatrix());
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Engine"))
        {
            ImGui::ColorEdit3("Clear Color", Engine::Get()->_clearColor.data());
            ImGui::TreePop();
        }
    }

    void DebugObject::Update(Time)
    {
        if (_kHandler->keydownThisFrame(DIK_F3))
            _displayDebug = !_displayDebug;

        if (_displayDebug)
        {
            ShowDebug();
            End();
        }
    }

    void DebugObject::ShowMatrix(Matrix4f matrix)
    {
        ImGui::Text("%f %f %f %f", matrix(0, 0), matrix(0, 1), matrix(0, 2), matrix(0, 3));
        ImGui::Text("%f %f %f %f", matrix(1, 0), matrix(1, 1), matrix(1, 2), matrix(1, 3));
        ImGui::Text("%f %f %f %f", matrix(2, 0), matrix(2, 1), matrix(2, 2), matrix(2, 3));
        ImGui::Text("%f %f %f %f", matrix(3, 0), matrix(3, 1), matrix(3, 2), matrix(3, 3));
    }

    void DebugObject::ShowVector(Vector3f vector)
    {
        ImGui::Text("%f %f %f", vector.x(), vector.y(), vector.z());
    }

    void DebugObject::ShowVector(Vector4f vector)
    {
        ImGui::Text("%f %f %f %f", vector.x(), vector.y(), vector.z(), vector.w());
    }

    void DebugObject::ShowQuaternion(Quaternionf quaternion)
    {
        ImGui::Text("%f %f %f %f", quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w());
    }

}
