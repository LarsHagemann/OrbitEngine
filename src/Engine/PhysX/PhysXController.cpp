#include "Engine/PhysX/PhysXController.hpp"
#include "Engine/Misc/Logger.hpp"

#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxExtensionsAPI.h>
#include <common/PxTolerancesScale.h>
#include <pvd/PxPvdTransport.h>

namespace orbit
{

    PhysXController::PhysXController() :
        m_foundation(PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback)),
        m_pvd(PxCreatePvd(*m_foundation))
    {
        ORBIT_INFO_LEVEL("Initializing Nvidia PhysX", 13);
        m_scale.speed = 1.0f;
        m_scale.length = 1.f;
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
        else
        {
            ORBIT_INFO("PhysX Visual Debugger connected on 127.0.0.1:5425");
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

#ifdef _DEBUG
        m_scene->getScenePvdClient()->setScenePvdFlags(
            PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS | 
            PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES | 
            PxPvdSceneFlag::eTRANSMIT_CONTACTS);
#endif

        m_controllerManager = PxCreateControllerManager(*m_scene);

        ORBIT_INFO_LEVEL("Nvidia PhysX initialized", 14);
    }

    PhysXController::~PhysXController()
    {
        //PxCloseExtensions();

        //m_physics->release();
        //m_pvd->release();
        //m_foundation->release();
    }

    void PhysXController::UpdatePhysX()
    {
        m_scene->simulate(1000.f / m_updatesPerSecond);
        m_scene->fetchResults(true);
    }

PxOrbitErrorCallback PhysXController::gErrorCallback;
PxDefaultAllocator PhysXController::gAllocator;

}