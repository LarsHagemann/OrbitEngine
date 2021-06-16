#pragma once
#include "Engine/Component/Component.hpp"
#include "Engine/Rendering/Mesh.hpp"
#include "Engine/PhysX/MaterialProperties.hpp"

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

        std::shared_ptr<Mesh> m_underlyingMesh;
    public:
        RigidStaticComponent(ObjectPtr object, std::shared_ptr<Mesh> mesh);
        ~RigidStaticComponent();

        static std::shared_ptr<RigidStaticComponent> create(ObjectPtr object, std::shared_ptr<Mesh> mesh);

        PxRigidStatic* GetBody() const { return m_rigidStaticBody; }

        void CookBody(Vector3f position, Quaternionf rotation, MaterialProperties material = MaterialProperties::DefaultMaterial(), Vector3f meshScale = Vector3f::Ones(), size_t vertexPositionOffset = 0);
    };

}