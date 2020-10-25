#include "Engine/Component/Component.hpp"
#include "Engine/Misc/Helper.hpp"

namespace orbit
{

	Component::Component(ObjectPtr object) :
		_boundObject(object),
		_flags(0)
	{
	}

	void Component::set_flag(size_t bitToBeModified)
	{
		orbit::set_flag(&_flags, (1 << bitToBeModified));
	}

	void Component::unset_flag(size_t bitToBeModified)
	{
		orbit::unset_flag(&_flags, (1 << bitToBeModified));
	}

	bool Component::GetFlag(size_t bitToCheck)
	{
		return orbit::is_flag_set(_flags, (1 << bitToCheck));
	}

}