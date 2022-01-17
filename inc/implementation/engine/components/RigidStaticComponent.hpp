#pragma once
#include "interfaces/engine/GameComponent.hpp"
#include "implementation/rendering/Mesh.hpp"
#include "implementation/rendering/Vertex.hpp"

#include <PxActor.h>
#include <PxShape.h>
#include <geometry/PxTriangleMesh.h>
#include <PxRigidStatic.h>

#include <unordered_map>
#include <memory>

namespace orbit
{

    using namespace physx;

    class RigidStaticComponent : public Physically
    {
    private:
        std::unordered_map<unsigned, std::unique_ptr<PxRigidStatic, PxDelete<PxRigidStatic>>> m_bodies;
        PxTriangleMeshGeometry m_geometry;
        std::unique_ptr<PxShape, PxDelete<PxShape>> m_shape;

        std::shared_ptr<Mesh<Vertex>> m_mesh;
        unsigned m_nextId;
    public:
        RigidStaticComponent(GameObject* boundObject, ResourceId meshId);
        ~RigidStaticComponent();
        // Does Physics updates in the background
        virtual void Update(size_t millis) override;
        // @brief: Creates a new rigid static component
        // @param boundObject: the object that is referenced by this component
        // @param mesh: the mesh for this rigid static body
        static std::shared_ptr<RigidStaticComponent> create(GameObject* boundObject, ResourceId meshId);

        void CookBody(MaterialProperties material_p, Vector3f meshScale, size_t vertexPositionOffset);
        unsigned AddActor(const Vector3f& position, const Quaternionf& rotation);
        unsigned AddActor(std::shared_ptr<Transform> transform);
    };

}