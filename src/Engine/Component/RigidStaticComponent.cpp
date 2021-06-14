#include "Engine/Component/RigidStaticComponent.hpp"

#include <cooking/PxTriangleMeshDesc.h>
#include <extensions/PxDefaultStreams.h>
#include <extensions/PxRigidActorExt.h>
#include <vector>

namespace orbit
{

    RigidStaticComponent::RigidStaticComponent(ObjectPtr object, std::shared_ptr<Mesh> mesh, size_t vertexPositionOffset) :
        Component(object)
    {
        const auto& vertexData = mesh->GetVertexData();
        const auto& indexData = mesh->GetIndexData();

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

        PxDefaultMemoryOutputStream writeBuffer;
        PxTriangleMeshCookingResult::Enum result;
        auto status = Engine::Get()->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
        if (status)
        {
            m_rigidStaticBody = Engine::Get()->GetPhysics()->createRigidStatic(PxTransform(PxVec3(0.f), PxQuat(0.f, PxVec3(0.f, 1.f, 0.f))));

            PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
            auto colliderMesh = Engine::Get()->GetPhysics()->createTriangleMesh(readBuffer);

            auto material = Engine::Get()->GetPhysics()->createMaterial(1.f, 1.f, .6f);
            m_geometry.triangleMesh = colliderMesh;
            m_geometry.scale = PxMeshScale(1.f);

            m_shape = PxRigidActorExt::createExclusiveShape(*m_rigidStaticBody, m_geometry, *material);

            Engine::Get()->GetPhysXScene()->addActor(*m_rigidStaticBody);
        }
        else
        {
            ORBIT_ERR("Failed to cook triangle mesh.");
        }
    }

    std::shared_ptr<RigidStaticComponent> RigidStaticComponent::create(ObjectPtr object, std::shared_ptr<Mesh> mesh, size_t vertexPositionOffset)
    {
        return std::make_shared<RigidStaticComponent>(object, mesh, vertexPositionOffset);
    }

}