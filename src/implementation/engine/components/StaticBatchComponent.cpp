#include "implementation/engine/components/StaticBatchComponent.hpp"

namespace orbit
{

    StaticBatchComponent::StaticBatchComponent(GameObject* object, ResourceId meshId) :
        BatchComponent(object, meshId)
    {
    }

    void StaticBatchComponent::Draw() const
    {
        if (!m_mesh || !m_transforms.size()) return;

        if (m_recacheNeccessary)
        {
            m_recacheNeccessary = false;
            m_transformBuffer.ResizeBuffer(m_transforms.size());
            auto i = 0u;
            for (auto transform : m_transforms)
                m_transformBuffer.SetVertex(i++, transform->LocalToWorldMatrix());

            m_transformBuffer.UpdateBuffer();
        }

        m_transformBuffer.Bind(1, sizeof(Matrix4f), 0);
        m_mesh->Bind();
        m_mesh->Draw(m_transforms.size());
    }

}