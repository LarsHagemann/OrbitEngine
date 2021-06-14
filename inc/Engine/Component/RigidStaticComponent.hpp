#pragma once
#include "Engine/Component/Component.hpp"
#include "Engine/Rendering/Mesh.hpp"

#include <PxRigidStatic.h>

namespace orbit
{

    using namespace physx;

    class RigidStaticComponent : public Component
    {
    protected:
        PxRigidStatic* m_rigidStaticBody;
        PxTriangleMeshGeometry m_geometry;
        PxShape* m_shape;
    public:
        RigidStaticComponent(ObjectPtr object, std::shared_ptr<Mesh> mesh, size_t vertexPositionOffset = 0);

        static std::shared_ptr<RigidStaticComponent> create(ObjectPtr object, std::shared_ptr<Mesh> mesh, size_t vertexPositionOffset = 0);

        PxRigidStatic* GetBody() const { return m_rigidStaticBody; }
    };

}