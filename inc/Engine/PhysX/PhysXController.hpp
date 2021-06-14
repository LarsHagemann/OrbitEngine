#pragma once
#include <characterkinematic/PxControllerManager.h>
#include <extensions/PxDefaultAllocator.h>
#include <PxPhysicsVersion.h>
#include <pvd/PxPvd.h>
#include <PxFoundation.h>
#include <PxPhysics.h>
#include <PxScene.h>
#include <cooking/PxCooking.h>

#include "PxOrbitErrorCallback.hpp"

namespace orbit
{

    using namespace physx;

    class PhysXController
    {
    protected:
        static PxOrbitErrorCallback gErrorCallback;
        static PxDefaultAllocator gAllocator;

        unsigned m_updatesPerSecond;

        PxControllerManager* m_controllerManager;
        PxFoundation* m_foundation;
        PxPvd* m_pvd;
        PxTolerancesScale m_scale;
        PxScene* m_scene;
        PxPhysics* m_physics;
        PxCooking* m_cooking;
    public:
        PhysXController();
        virtual ~PhysXController();

        PxPhysics* GetPhysics() const { return m_physics; }
        PxCooking* GetCooking() const { return m_cooking; }
        PxScene* GetPhysXScene() const { return m_scene; }
        PxControllerManager* GetControllerManager() const { return m_controllerManager; }

        void UpdatePhysX();
    };

}