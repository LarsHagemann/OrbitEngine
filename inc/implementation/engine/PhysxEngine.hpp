#pragma once
#include <characterkinematic/PxControllerManager.h>
#include <extensions/PxDefaultAllocator.h>
#include <PxPhysicsVersion.h>
#include <pvd/PxPvd.h>
#include <PxFoundation.h>
#include <PxPhysics.h>
#include <PxScene.h>
#include <cooking/PxCooking.h>

#include <type_traits>
#include <memory>

namespace orbit
{

    using namespace physx;

    struct MaterialProperties
    {
        float staticFriction;
        float dynamicFriction;
        float restitution;
        static MaterialProperties DefaultMaterial()
        {
            return MaterialProperties{
                0.5f,
                0.5f,
                0.1f
            };
        }
    };

    template <class _Ty>
    struct PxDelete { // PhysX deleter for unique_ptr
        constexpr PxDelete() noexcept = default;

        template <class _Ty2, std::enable_if_t<std::is_convertible_v<_Ty2*, _Ty*>, int> = 0>
        PxDelete(const PxDelete<_Ty2>&) noexcept {}

        void operator()(_Ty* _Ptr) const noexcept /* strengthened */ { // delete a pointer
            //if (_Ptr)
            //    _Ptr->release();
            _Ptr = nullptr;
        }
    };

    template<typename PxT>
    using PxUPtr = std::unique_ptr<PxT, PxDelete<PxT>>;

    class PxOrbitErrorCallback : public PxErrorCallback
    {
    public:
        virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;
    };

    class PhysxEngine
    {
    private:
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
        PhysxEngine();
        virtual ~PhysxEngine();

        PxPhysics* GetPhysics() const { return m_physics; }
        PxCooking* GetCooking() const { return m_cooking; }
        PxScene* GetPhysXScene() const { return m_scene; }
        PxControllerManager* GetControllerManager() const { return m_controllerManager; }

        void Cleanup();

        void UpdatePhysX(); 
    };

}
