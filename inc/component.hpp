#pragma once
#include <memory>

namespace orbit
{

	class Engine;
	class Object;
	using EnginePtr = std::shared_ptr<Engine>;
	using ObjectPtr = std::shared_ptr<Object>;

	// @brief:
	//	All behaviour is based on components that can be added to objects
	//	A RenderComponent will, for example, look for renderable components
	//	and draw them to the screen if possible.
	//	A BatchComponent is such a renderable component (see class Renderable in renderable.hpp for more
	//	or class BatchComponent in batch_component.hpp)
	class Component
	{
	protected:
		// @member: shared pointer to the game engine object
		EnginePtr _engine;
		// @member: shared pointer to the object this component is bound to	
		ObjectPtr _boundObject;
	public:
		// constructor takes the engine and the object it is bound to
		Component(EnginePtr engine, ObjectPtr boundObject);
		// virtual destructor
		virtual ~Component() {}
	};

	using ComponentPtr = std::shared_ptr<Component>;

}
