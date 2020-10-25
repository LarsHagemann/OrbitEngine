#pragma once
#include "Component.hpp"
#include "Engine/Misc/Time.hpp"

namespace orbit
{

	class EventDriven
	{
	public:
		virtual void Update(Time dt) = 0;
	};

}
