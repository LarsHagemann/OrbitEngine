#include "3DView.hpp"

#include "implementation/rendering/Mesh.hpp"
#include "implementation/engine/Engine.hpp"

void O3DView::Init()
{
    m_focus = std::make_shared<orbit::Transform>();
    m_camera = std::make_shared<orbit::ArcBallCamera>();
    m_camera->SetDistance(3.f);
    m_camera->SetTarget(m_focus);

    m_keyboard = AddComponent<orbit::KeyboardComponent>("keyboard");
    m_mouse = AddComponent<orbit::MouseComponent>("mouse");
    m_batch = AddComponent<orbit::StaticBatchComponent>("batch", 0);
    m_batch->AddTransform(std::make_shared<orbit::Transform>());

    auto b2 = AddComponent<orbit::StaticBatchComponent>("batch__arrow", ENGINE->RMGetIdFromName("OrbViewer__Arrows"));
    b2->GetMesh()->GetSubmesh(0).pipelineStateId = ENGINE->RMGetIdFromName("pipeline_states/material_color");
    b2->AddTransform(std::make_shared<orbit::Transform>());
}

void O3DView::Update(const orbit::Time& dt)
{
    orbit::GameObject::Update(dt);

    if (m_mouse->buttonDown(orbit::MouseComponent::MouseButton::Middle))
    {
        if (m_mouse->mouseMoved())
        {
            m_camera->Tilt(m_mouse->mousePositionDelta().y() * dt.asSeconds());
            m_camera->Pan(m_mouse->mousePositionDelta().x() * dt.asSeconds());
        }
    }
    if (m_mouse->mouseWheelMoved())
    {
        m_camera->Zoom((m_mouse->wheelDelta() > 0 ? .95f : 1.1f));
    }
}

void O3DView::LoadGeometry(const std::string& id)
{
    auto meshId = ENGINE->RMGetIdFromName(id);
    auto mesh = ENGINE->RMLoadResource<orbit::Mesh<orbit::Vertex>>(meshId);

    m_batch->SetMesh(mesh);
}

void O3DView::LoadTexture(const std::string& id)
{
    //auto meshId = ENGINE->RMGetIdFromName("OrbViewer__Plane");
    //auto mesh = ENGINE->RMLoadResource<orbit::Mesh<orbit::Vertex>>(meshId);
    //
    //m_batch->SetMesh(mesh);
}

void O3DView::LoadMaterial(const std::string& id)
{
    auto meshId = ENGINE->RMGetIdFromName("OrbViewer__Sphere");
    auto mesh = ENGINE->RMLoadResource<orbit::Mesh<orbit::Vertex>>(meshId);

    mesh->GetSubmesh(0).materialId = ENGINE->RMGetIdFromName(id);

    m_batch->SetMesh(mesh);
}

