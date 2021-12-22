#include "implementation/engine/GameObject.hpp"

namespace orbit
{

    void GameObject::Update(const Time& dt)
    {
        for (auto& component : m_components)
        {
            if (component.second->test(ComponentFlags::F_UPDATABLE))
                std::static_pointer_cast<Updatable>(component.second)->Update(dt);
        }
    }
    
    void GameObject::PhysicsUpdate(size_t millis)
    {
        for (auto& component : m_components)
        {
            if (component.second->test(ComponentFlags::F_PHYSICS))
                std::static_pointer_cast<Physically>(component.second)->Update(millis);
        }
    }

    void GameObject::Draw() const
    {
        for (auto& component : m_components)
        {
            if (component.second->test(ComponentFlags::F_RENDERABLE))
                std::static_pointer_cast<Renderable>(component.second)->Draw();
        }
    }

}