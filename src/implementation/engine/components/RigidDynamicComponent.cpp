#include "implementation/engine/components/RigidDynamicComponent.hpp"
#include "implementation/misc/Logger.hpp"

#include <extensions/PxDefaultStreams.h>
#include <extensions/PxRigidActorExt.h>
#include <PxMaterial.h>

#define PX_RELEASE(x) if(x) { x->release(); x = nullptr; }

namespace orbit
{
    
    RigidDynamicComponent::RigidDynamicComponent(GameObject* boundObject, ResourceId meshId) :
        Physically(boundObject)
    {
        m_mesh = std::make_shared<Mesh<Vertex>>();
        m_mesh->SetId(meshId);
        m_mesh->Load();
    }

    RigidDynamicComponent::~RigidDynamicComponent()
    {
        ORBIT_INFO_LEVEL(ORBIT_LEVEL_DEBUG, "Releasing RigidStaticComponent");
        std::vector<PxMaterial*> materials;
        materials.resize(m_shape->getNbMaterials());
        m_shape->getMaterials(materials.data(), materials.size());
        PX_RELEASE(m_shape);
        for (auto material : materials)
        {
            PX_RELEASE(material);
        }
        materials.clear();
        m_bodies.clear();
        m_nextId = 0;
    }

    void RigidDynamicComponent::Update(size_t millis)
    {
        for (auto& [id, body] : m_bodies)
        {
            if (!body->isSleeping())
            {
                m_transforms[id]->SetTranslation(orbit::Math<float>::PxToEigen(body->getGlobalPose().p));
                m_transforms[id]->SetRotation(orbit::Math<float>::PxToEigen(body->getGlobalPose().q));
            }
        }
    }

    std::shared_ptr<RigidDynamicComponent> RigidDynamicComponent::create(GameObject* boundObject, ResourceId meshId)
    {
        return std::make_shared<RigidDynamicComponent>(boundObject, meshId);
    }

    void RigidDynamicComponent::CookBody(MaterialProperties material_p, Vector3f meshScale, size_t vertexPositionOffset)
    {
        const auto& vertexData = m_mesh->GetVertexBuffer();
        const auto& indexData = m_mesh->GetIndexBuffer();

        std::vector<Vector3f> colliderPositions;
        colliderPositions.reserve(vertexData->NumVertices());
        const auto stride = vertexData->GetBufferSize() / vertexData->NumVertices();
        for (auto i = 0u; i < vertexData->NumVertices(); ++i)
        {
            auto position = vertexData->GetVertices().at(i).position;
            colliderPositions.emplace_back(position);
        }

        PxTriangleMeshDesc meshDesc;
        meshDesc.points.count = static_cast<PxU32>(colliderPositions.size());
        meshDesc.points.stride = sizeof(Vector3f);
        meshDesc.points.data = colliderPositions.data();

        meshDesc.triangles.count = static_cast<PxU32>(indexData->NumIndices() / 3);
        meshDesc.triangles.stride = 3 * sizeof(uint32_t);
        meshDesc.triangles.data = indexData->GetIndices().data();

        PxDefaultMemoryOutputStream writeBuffer;
        PxTriangleMeshCookingResult::Enum result;
        auto status = Engine::Get()->GetCooking()->cookTriangleMesh(meshDesc, writeBuffer, &result);
        if (status)
        {
            PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
            auto colliderMesh = Engine::Get()->GetPhysics()->createTriangleMesh(readBuffer);

            auto material = Engine::Get()->GetPhysics()->createMaterial(
                material_p.staticFriction, 
                material_p.dynamicFriction, 
                material_p.restitution
            );
            m_geometry.triangleMesh = colliderMesh;
            m_geometry.scale = PxMeshScale(Math<float>::EigenToPx3(meshScale));

            m_shape = std::unique_ptr<PxShape, PxDelete<PxShape>>(ENGINE->GetPhysics()->createShape(
                m_geometry, 
                *material, 
                false,
                PxShapeFlag::eSCENE_QUERY_SHAPE));
        }
        else
        {
            ORBIT_ERROR("Failed to cook triangle mesh.");
        }
    }

    unsigned RigidDynamicComponent::AddActor(std::shared_ptr<Transform> transform)
    {
        auto body = ENGINE->GetPhysics()->createRigidDynamic(PxTransform(
                Math<float>::EigenToPx3(transform->GetCombinedTranslation()),
                Math<float>::EigenToPx(Quaternionf(transform->GetCombinedRotation()))));
        
        auto id = m_nextId++;
        m_bodies[id] = std::unique_ptr<PxRigidDynamic, PxDelete<PxRigidDynamic>>(body);
        m_transforms[id] = transform;
        body->attachShape(*m_shape);
        body->setMass(0.5f);
        ENGINE->GetPhysXScene()->addActor(*body);
        return id;
    }

}