#include "implementation/engine/components/BatchComponent.hpp"

namespace orbit
{

    BatchComponent::BatchComponent(GameObject* object, ResourceId meshId) :
        Renderable(object)
    {
        m_mesh = std::make_shared<Mesh<Vertex>>();
        m_mesh->SetId(meshId);
        m_mesh->Load();
    }

    TransformPtr BatchComponent::AddTransform(TransformPtr transform)
    {
        m_transforms.emplace_back(transform);
        return transform;
    }

    void BatchComponent::Draw() const
    {
        if (!m_mesh) return;

        VertexBuffer<Matrix4f> transformBuffer;
        transformBuffer.ResizeBuffer(m_transforms.size());
        auto i = 0u;
        for (auto transform : m_transforms)
            transformBuffer.SetVertex(i++, transform->LocalToWorldMatrix());

        transformBuffer.UpdateBuffer();
        transformBuffer.Bind(1, sizeof(Matrix4f), 0);

        m_mesh->Bind();
        m_mesh->Draw(m_transforms.size());        
    }
    
}