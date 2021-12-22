#pragma once
#include "implementation/engine/components/BatchComponent.hpp"

namespace orbit
{

    class StaticBatchComponent : public BatchComponent
    {
    private:
        mutable VertexBuffer<Eigen::Matrix4f> m_transformBuffer;
        mutable bool m_recacheNeccessary = true;
    public:
        StaticBatchComponent(GameObject* object, ResourceId meshId);
        void UpdateBuffer() { m_recacheNeccessary = true; }
        virtual void Draw() const override;
    };

}