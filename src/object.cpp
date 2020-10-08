#include "object.hpp"

namespace orbit
{

	void Object::Init(EnginePtr engine)
	{
		_engine = engine;
		_active = true; // objects are active by default
	}

	void Object::SetActive(bool active)
	{
		_active = active;
	}

	bool Object::IsActive()
	{
		return _active;
	}

	std::shared_ptr<Component> Object::Get(std::string_view id)
	{
		auto pos = _components.find(std::string(id));
		if (pos == _components.end())
			return nullptr; // component does not exist

		return pos->second;
	}

}