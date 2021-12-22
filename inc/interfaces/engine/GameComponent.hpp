#pragma once
#include "implementation/misc/Time.hpp"
#include "implementation/misc/BitField.hpp"

#include <memory>

namespace orbit
{

    class GameObject;
    using GObjectPtr = std::shared_ptr<GameObject>;

    constexpr static auto NumFlagBits = 32U;

    enum class ComponentFlags : uint32_t
    {
        F_RENDERABLE = (1 << 0),
        F_UPDATABLE  = (1 << 1),
        F_PHYSICS    = (1 << 2),
    };

    class IComponent : public BitField<NumFlagBits>
    {
    protected:
        GameObject* m_object;
    public:
        template<typename...T>
        IComponent(GameObject* object, T...flags) : 
            m_object(object),
            BitField<NumFlagBits>(flags...)
        {}
        IComponent(GameObject* object) :
            m_object(object),
            BitField<NumFlagBits>()
        {}
    };

    class Renderable : public IComponent
    {
    protected:
    public:
        Renderable(GameObject* object) :
            IComponent(object, ComponentFlags::F_RENDERABLE)
        {}
        virtual void Draw() const = 0;
    };

    class Updatable : public IComponent
    {
    protected:
    public:
        Updatable(GameObject* object) :
            IComponent(object, ComponentFlags::F_UPDATABLE)
        {}
        virtual void Update(const Time& dTime) = 0;
    };

    class Physically : public IComponent
    {
    protected:
    public:
        Physically(GameObject* object) :
            IComponent(object, ComponentFlags::F_PHYSICS)
        {}
        virtual void Update(size_t millis) = 0;
    };

}