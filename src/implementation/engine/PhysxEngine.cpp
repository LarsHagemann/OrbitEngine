#include "implementation/engine/PhysxEngine.hpp"
#include "implementation/misc/Logger.hpp"
#include "implementation/Debug.hpp"

#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultCpuDispatcher.h>
#include <extensions/PxDefaultSimulationFilterShader.h>
#include <extensions/PxExtensionsAPI.h>
#include <common/PxTolerancesScale.h>
#include <pvd/PxPvdTransport.h>

#include <cassert>

#define PX_RELEASE(x) if(x) { x->release(); x = nullptr; }

namespace orbit
{

    void PxOrbitErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line)
    {
        const char* errorCode = nullptr;
        switch (code)
        {
        case PxErrorCode::eNO_ERROR:
            errorCode = "no error";
            break;
        case PxErrorCode::eINVALID_PARAMETER:
            errorCode = "invalid parameter";
            break;
        case PxErrorCode::eINVALID_OPERATION:
            errorCode = "invalid operation";
            break;
        case PxErrorCode::eOUT_OF_MEMORY:
            errorCode = "out of memory";
            break;
        case PxErrorCode::eDEBUG_INFO:
            errorCode = "info";
            break;
        case PxErrorCode::eDEBUG_WARNING:
            errorCode = "warning";
            break;
        case PxErrorCode::ePERF_WARNING:
            errorCode = "performance warning";
            break;
        case PxErrorCode::eABORT:
            errorCode = "abort";
            break;
        case PxErrorCode::eINTERNAL_ERROR:
            errorCode = "internal error";
            break;
        case PxErrorCode::eMASK_ALL:
            errorCode = "unknown error";
            break;
        }

        assert(errorCode != nullptr && "Unsupported error code.");

        ORBIT_LOG("{PhysX: %s} %s", errorCode, message);

        switch (code)
        {
        case PxErrorCode::eINVALID_PARAMETER:
        case PxErrorCode::eINVALID_OPERATION:
        case PxErrorCode::eOUT_OF_MEMORY:
        case PxErrorCode::eABORT:
        case PxErrorCode::eINTERNAL_ERROR:
            ORBIT_DEBUG_BREAK;
        }

        if (code == PxErrorCode::eABORT)
            ORBIT_THROW("An unrecoverable error has occurred in Nvidia PhysX.");
    }

    PhysxEngine::PhysxEngine() :
        m_scale(PxTolerancesScale()),
        m_foundation(nullptr),
        m_physics(nullptr),
        m_cooking(nullptr),
        m_controllerManager(nullptr),
        m_scene(nullptr),
        m_pvd(nullptr),
        m_updatesPerSecond(60u)
    {
        ORBIT_INFO_LEVEL(ORBIT_LEVEL_DEBUG, "Initializing NVidia PhysX");
        // Initialize Foundation
        m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
        if (!m_foundation)
        {
            ORBIT_ERROR("Failed to initialize Nvidia PhysX");
            return;
        }

        // Initialize Visual debugger client
        m_pvd = PxCreatePvd(*m_foundation);
        if (!m_pvd)
        {
            ORBIT_ERROR("Failed to initialize Nvidia PhysX");
            return;
        }
#ifdef _DEBUG
        auto PVD_HOST = "127.0.0.1";
        auto transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
        if (!m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL))
        {
            ORBIT_LOG("PhysX Visual Debugger not connected.");
        }
        else
        {
            ORBIT_LOG("PhysX Visual Debugger connected on 127.0.0.1:5425");
        }
#endif

        m_scale.speed = 1.0f;
        m_scale.length = 1.f;

        // Initialize Physics
        m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, m_scale, true, m_pvd);
        if (!m_physics)
        {
            ORBIT_LOG("Failed to initialize Nvidia PhysX");
            return;
        }

        // Initialize Cooking
        m_cooking = PxCreateCooking(PX_PHYSICS_VERSION, *m_foundation, PxCookingParams(m_scale));
        if (!m_cooking)
        {
            ORBIT_LOG("Failed to initialize Nvidia PhysX");
            return;
        }

        PxSceneDesc sceneDesc(m_scale);
        sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
        sceneDesc.gravity = PxVec3(0.f, 0.f, 0.f);
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        sceneDesc.bounceThresholdVelocity = 1.9f;

        // Initialize scene
        m_scene = m_physics->createScene(sceneDesc);
        if (!m_scene)
        {
            ORBIT_ERROR("Failed to initialize Nvidia PhysX");
            return;
        }

#ifdef _DEBUG
        m_scene->getScenePvdClient()->setScenePvdFlags(
            PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS | 
            PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES | 
            PxPvdSceneFlag::eTRANSMIT_CONTACTS);
#endif

        // Initialize controller manager
        m_controllerManager = PxCreateControllerManager(*m_scene);
        if (!m_controllerManager)
        {
            ORBIT_ERROR("Failed to initialize Nvidia PhysX");
            return;
        }

        ORBIT_INFO_LEVEL(ORBIT_LEVEL_DEBUG, "Nvidia PhysX initialized");
    }

    PhysxEngine::~PhysxEngine()
    {        
    }

    void PhysxEngine::Cleanup()
    {
        m_controllerManager->purgeControllers();
        PX_RELEASE(m_controllerManager);

        PX_RELEASE(m_scene);
        PX_RELEASE(m_cooking);

        PX_RELEASE(m_physics);
        if (m_pvd)
        {
            PxPvdTransport* transport = m_pvd->getTransport();
            m_pvd->release();	m_pvd = NULL;
            PX_RELEASE(transport);
        }

        PX_RELEASE(m_foundation);
    }

    void PhysxEngine::UpdatePhysX()
    {
        m_scene->simulate(1000.f / m_updatesPerSecond);
        m_scene->fetchResults(true);
    }

PxOrbitErrorCallback PhysxEngine::gErrorCallback;
PxDefaultAllocator PhysxEngine::gAllocator;

}
