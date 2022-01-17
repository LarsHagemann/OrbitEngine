#include "implementation/engine/components/RigidStaticComponent.hpp"
#include "implementation/misc/Logger.hpp"

#include <extensions/PxDefaultStreams.h>
#include <extensions/PxRigidActorExt.h>
#include <PxMaterial.h>

#define PX_RELEASE(x) if(x) { x->release(); x = nullptr; }

namespace orbit
{
    
    RigidStaticComponent::RigidStaticComponent(GameObject* boundObject, ResourceId meshId) :
        Physically(boundObject)
    {
        m_mesh = std::make_shared<Mesh>();
        m_mesh->SetId(meshId);
        m_mesh->Load();
    }

    RigidStaticComponent::~RigidStaticComponent()
    {
        std::vector<PxMaterial*> materials;
        materials.resize(m_shape->getNbMaterials());
        m_shape->getMaterials(materials.data(), materials.size());
        PX_RELEASE(m_shape);
        for (auto material : materials)
        {
            PX_RELEASE(material);
        }
        materials.clear();
        for (auto it = m_bodies.begin(); it != m_bodies.end(); ++it)
        {
            if (it->second.get())
                it->second->release();
        }
        m_bodies.clear();
        m_nextId = 0;
    }

    void RigidStaticComponent::Update(size_t millis)
    {

    }

    std::shared_ptr<RigidStaticComponent> RigidStaticComponent::create(GameObject* boundObject, ResourceId meshId)
    {
        return std::make_shared<RigidStaticComponent>(boundObject, meshId);
    }

    void RigidStaticComponent::CookBody(MaterialProperties material_p, Vector3f meshScale, size_t vertexPositionOffset)
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

            m_shape = std::unique_ptr<PxShape, PxDelete<PxShape>>(ENGINE->GetPhysics()->createShape(m_geometry, *material));
        }
        else
        {
            ORBIT_ERROR("Failed to cook triangle mesh.");
        }
    }

    unsigned RigidStaticComponent::AddActor(const Vector3f& position, const Quaternionf& rotation)
    {
        auto body = Engine::Get()->GetPhysics()->createRigidStatic(PxTransform(
                Math<float>::EigenToPx3(position),
                Math<float>::EigenToPx(rotation)));
        
        auto id = m_nextId++;
        m_bodies[id] = std::unique_ptr<PxRigidStatic, PxDelete<PxRigidStatic>>(body);
        body->attachShape(*m_shape);
        ENGINE->GetPhysXScene()->addActor(*body);
        return id;
    }

    unsigned RigidStaticComponent::AddActor(std::shared_ptr<Transform> transform)
    {
        return AddActor(transform->GetCombinedTranslation(), Quaternionf(transform->GetCombinedRotation()));
    }

}