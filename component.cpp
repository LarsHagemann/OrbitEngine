#include "component.hpp"

namespace orbit
{

	Component::Component(EnginePtr engine, ObjectPtr boundObject) : 
		_boundObject(boundObject),
		_engine(engine)
	{
	}

}