#pragma once
#include "Engine/Component/Component.hpp"
#include "Engine/Engine.hpp"

#include <characterkinematic/PxCapsuleController.h>

namespace orbit
{

    using namespace physx;

    template<class Controller = PxCapsuleController, class ControllerDesc = PxCapsuleControllerDesc>
    class EntityControllerComponent : public Component
    {
    protected:
        PxController* m_controller;
    public:
        EntityControllerComponent(ObjectPtr object, const ControllerDesc& desc) :
            Component(object)
        {
            m_controller = Engine::Get()->GetControllerManager()->createController(desc);
            m_controller->setUserData(object.get());
        }

        static std::shared_ptr<EntityControllerComponent> create(ObjectPtr object, const ControllerDesc& desc)
        {
            return std::make_shared<EntityControllerComponent>(object, desc);
        }

        PxController* GetController() const { return m_controller; }
    };

}