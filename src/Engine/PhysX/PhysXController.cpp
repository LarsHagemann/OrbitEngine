#include "Engine/PhysX/PhysXController.hpp"
#include "Engine/Misc/Logger.hpp"

#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <common/PxTolerancesScale.h>
#include <pvd/PxPvdTransport.h>


namespace orbit
{

    PhysXController::PhysXController() :
        m_foundation(PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback)),
        m_pvd(PxCreatePvd(*m_foundation))
    {
        m_scale.speed = 4.0f;
        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, m_scale);
        m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(m_scale));
        if (!m_foundation || !m_pvd || !m_cooking)
        {
            ORBIT_ERR("Failed to initialize Nvidia PhysX");
            return;
        }
        
#ifdef _DEBUG
        auto PVD_HOST = "127.0.0.1";
        auto transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        if (!m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL))
        {
            ORBIT_INFO("PhysX Visual Debugger not connected.");
        }
#endif

        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, m_scale, true, m_pvd);

        if (!m_physics)
        {
            ORBIT_ERR("Failed to initialize Nvidia PhysX");
            return;
        }

        PxSceneDesc sceneDesc(m_scale);
        sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
        sceneDesc.gravity = PxVec3(0.f, 0.f, 0.f);
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        sceneDesc.bounceThresholdVelocity = 1.9f;

        m_scene = m_physics->createScene(sceneDesc);

        if (!m_scene)
        {
            ORBIT_ERR("Failed to initialize Nvidia PhysX");
            return;
        }

        m_controllerManager = PxCreateControllerManager(*m_scene);
    }

PxOrbitErrorCallback PhysXController::gErrorCallback;
PxDefaultAllocator PhysXController::gAllocator;

}