#pragma once
#include "implementation/misc/Transform.hpp"
#include "implementation/rendering/Mesh.hpp"
#include "interfaces/engine/GameComponent.hpp"

#include <vector>

namespace orbit
{

    class BatchComponent : public Renderable
    {
    protected:
        SPtr<Mesh> m_mesh;
        std::vector<TransformPtr> m_transforms;
    public:
        BatchComponent(GameObject* object, ResourceId meshId);
        TransformPtr AddTransform(TransformPtr transform);
        virtual void Draw() const override;
        SPtr<Mesh> GetMesh() const { return m_mesh; }
        void SetMesh(SPtr<Mesh> mesh) { m_mesh = mesh; }
    };

}