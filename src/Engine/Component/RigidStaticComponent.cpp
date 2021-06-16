#include "Engine/Component/RigidStaticComponent.hpp"
#include "Engine/PhysX/PhysX.hpp"
#include <cooking/PxTriangleMeshDesc.h>
#include <extensions/PxDefaultStreams.h>
#include <extensions/PxRigidActorExt.h>
#include <PxMaterial.h>
#include <vector>

namespace orbit
{

    RigidStaticComponent::RigidStaticComponent(ObjectPtr object, std::shared_ptr<Mesh> mesh) :
        Component(object),
        m_underlyingMesh(mesh)
    {
    }

    RigidStaticComponent::~RigidStaticComponent()
    {
        ORBIT_INFO_LEVEL("Releasing RigidStaticComponent", 20);
        std::vector<PxMaterial*> materials;
        materials.resize(m_shape->getNbMaterials());
        m_shape->getMaterials(materials.data(), materials.size());
        PX_RELEASE(m_shape);
        for (auto material : materials)
        {
            PX_RELEASE(material);
        }
        materials.clear();
        PX_RELEASE(m_rigidStaticBody);
    }

    std::shared_ptr<RigidStaticComponent> RigidStaticComponent::create(ObjectPtr object, std::shared_ptr<Mesh> mesh)
    {
        return std::make_shared<RigidStaticComponent>(object, mesh);
    }

    void RigidStaticComponent::CookBody(Vector3f position, Quaternionf rotation, MaterialProperties material_p, Vector3f meshScale, size_t vertexPositionOffset)
    {
        const auto& vertexData = m_underlyingMesh->GetVertexData();
        const auto& indexData = m_underlyingMesh->GetIndexData();

        std::vector<Vector3f> colliderPositions;
        colliderPositions.reserve(vertexData.GetVertexCount());
        for (auto i = 0u; i < vertexData.GetVertexCount(); ++i)
        {
            auto positionOffset = i * vertexData.GetStride() + vertexPositionOffset;
            auto position = *(Vector3f*)((const char*)vertexData.GetBuffer().memory + positionOffset);

            colliderPositions.emplace_back(position);
        }

        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = static_cast<PxU32>(colliderPositions.size());
        meshDesc.points.stride = sizeof(Vector3f);
        meshDesc.points.data = colliderPositions.data();

        meshDesc.triangles.count = static_cast<PxU32>(indexData.size() / 3);
        meshDesc.triangles.stride = 3 * sizeof(uint16_t);
        meshDesc.triangles.data = indexData.indices.data();
        if (indexData.is16Bit)
            meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;

        PxDefaultMemoryOutputStream writeBuffer;
        PxTriangleMeshCookingResult::Enum result;
        auto status = Engine::Get()->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
        if (status)
        {
            m_rigidStaticBody = Engine::Get()->GetPhysics()->createRigidStatic(PxTransform(
                Math<>::EigenToPx3(position),
                Math<>::EigenToPx(rotation)));

            PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
            auto colliderMesh = Engine::Get()->GetPhysics()->createTriangleMesh(readBuffer);

            auto material = Engine::Get()->GetPhysics()->createMaterial(
                material_p.staticFriction, 
                material_p.dynamicFriction, 
                material_p.restitution
            );
            m_geometry.triangleMesh = colliderMesh;
            m_geometry.scale = PxMeshScale(Math<>::EigenToPx3(meshScale));

            m_shape = PxRigidActorExt::createExclusiveShape(*m_rigidStaticBody, m_geometry, *material);

            Engine::Get()->GetPhysXScene()->addActor(*m_rigidStaticBody);
        }
        else
        {
            ORBIT_ERR("Failed to cook triangle mesh.");
        }
    }

}