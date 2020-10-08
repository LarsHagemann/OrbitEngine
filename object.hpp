#pragma once
#include <unordered_map>
#include <string_view>

#include "component.hpp"
#include "time.hpp"

namespace orbit
{

	// @brief: An Object encapsulates certain behaviour
	//	(for example there would be a player class inheriting from
	//	Object to encapsulate controller movement and rendering)
	class Object : public std::enable_shared_from_this<Object>
	{
	protected:
		friend class Engine;
		// @member: hashtable of components
		//	the player class would for example would have a BatchComponent, a RenderComponent,
		//	a MouseComponent and a KeyboardComponent
		//		{ "batch/player", shared_ptr<BatchComponent> }
		//		{ "mouse/player", shared_ptr<MouseComponent> }
		//		{ "keyboard/player", shared_ptr<KeyboardComponent> }
		//	(see batch_component.hpp, mouse_component.hpp and 
		//	keyboard_component.hpp respectively)
		std::unordered_map<std::string, ComponentPtr> _components;
		// @member: mirrors the active status of the object
		//	An inactive object will neither be rendered nor updated
		bool _active;
		// @member: shared pointer to the game engine object
		EnginePtr _engine;
	public:
		// @method: objects need to be initialized after the constructor has been
		//	called, for shared_from_this() to work
		virtual void Init(EnginePtr engine);
		// @method: this function is called every frame (if the object is active)
		//	for updating the internal state.
		// @param dTime: time elapsed since the last frame.
		virtual void Update(Time dTime) {}
		// @method: sets the active status of the object
		// @param active: the new active status of the object
		//	inactive objects (objects with SetActive(false)) do not
		//	get rendered or updated by the engine
		void SetActive(bool active = true);
		// @method: is the object active?
		// @return: active status of the object
		bool IsActive();
		// @method: adds a component to the object
		// @brief: For most components the call will look like
		//		AddComponent<MyComponent>("my/component", _engine);
		//	But certain components need additional parameters, thus there is a variadic
		//	template argument to supply those parameters.
		//		AddComponent<MyComponent2>("my/component2", _engine, _veryImportantParameter);
		template<class ComponentDerived, class ...Args>
		std::shared_ptr<ComponentDerived> AddComponent(std::string_view id, EnginePtr engine, Args... args)
		{
			auto pos = _components.find(std::string(id));
			if (pos != _components.end()) // component with same name is already enlisted
				return nullptr;

			// create the new component and
			// emplace it into the hashtable
			auto ptr = ComponentDerived::create(engine, shared_from_this(), args...);
			_components.emplace(std::string(id), ptr);
			return ptr;
		}

		// @method: returns the Component with id
		// @brief: this function does a dynamic (runtime) type check and returns a nullptr if
		//	the requested type does not match
		//	If you are certain, that a component with the corresponding id has a specific type
		//	it is recommended to use Object::GetStatic(id) instead, since that skips the runtime
		//	type check. If you don't know about the object's type use GetDynamic.
		template<class ComponentDerived>
		std::shared_ptr<ComponentDerived> GetDynamic(std::string_view id)
		{
			return std::dynamic_pointer_cast<ComponentDerived>(Get(id));
		}

		// @method: returns the Component with id
		// @brief: this funcion does no runtime type check, so use this function only if you know
		//	the type of the component
		template<class ComponentDerived>
		std::shared_ptr<ComponentDerived> GetStatic(std::string_view id)
		{
			return std::static_pointer_cast<ComponentDerived>(Get(id));
		}

		// @method: returns the Component with id
		// @brief: this function simply fetches the component from the 
		//	hashtable and returns nullptr if the id cannot be found
		std::shared_ptr<Component> Get(std::string_view id);
	};

}
