#pragma once
#include "implementation/misc/Time.hpp"
#include "interfaces/misc/UnLoadable.hpp"
#include "interfaces/engine/GameComponent.hpp"

#include <unordered_map>
#include <memory>
#include <string>

namespace orbit
{

    // Describes a single entity in the game. An entity can contain
    // multiple components that are responsible for certain tasks
    class GameObject : public UnLoadable
    {
    protected:
        friend class EngineBase;
        // @member: identifier in the scene
        std::string m_identifier;
        // @member: true if this object is active
        bool m_isActive = true;
        // @member: set of components of this game object
        std::unordered_map<std::string, std::shared_ptr<IComponent>> m_components;
        //<-- Maybe cache components of certain types (updatable, physically and renderable)
        //<-- so that they can be accessed faster when needed
    public:
        virtual ~GameObject() {}
        // @method: called once per frame to perform update calculations
        // @param dt: time elapsed since the frame began
        virtual void Update(const Time& dt);
        // @method: Make updates that depend on physics here. 
        // @param millis: milliseconds elapsed since the physics update began
        virtual void PhysicsUpdate(size_t millis);
        // @method: Draws this object to the screen
        virtual void Draw() const;
        // @method: initializes an object
        virtual void Init() {}

        // @method: Returns true if this object is at moment
        bool IsActive() const { return m_isActive; }
        // @method: Updates the active state of this object
        // @param activeState: the new active state of the object (true means the object is active)
        void Activate(bool activeState = true) { m_isActive = activeState; }

        // @method: Sets the objects identifier
        // @param identifier: The object's identifier
        // @internal
        void SetIdentifier(const std::string& identifier) { m_identifier = identifier; }

        // @method: Adds a component to this object
        template<typename Component, typename...Params>
        std::shared_ptr<Component> AddComponent(const std::string& identifier, Params...params)
        {
            auto com = std::make_shared<Component>(this, params...);
            m_components.emplace(identifier, com);
            return com;
        }
        // @method: Returns a component by its identifier
        // @return: nullptr if the component is not of type <Component> or
        //  if the identifier does not belong to any known component
        // @template Component: The type of the component
        // @param identifier: identifier of the component
        template<typename Component>
        std::shared_ptr<Component> GetComponent(const std::string& identifier)
        {
            auto com = m_components.find(identifier);
            if (com == m_components.end())
                return nullptr;
            
            try {
                return
#ifdef _DEBUG
                    std::dynamic_pointer_cast<Component>(com->second)
#else
                    std::static_pointer_cast<Component>(com->second)
#endif
                    ;
            }
            catch(...) {
                return nullptr;
            }
        }
    };

}