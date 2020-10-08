#pragma once
#include "helper.hpp"
#include "time.hpp"

namespace orbit
{

	// @brief: Components inheriting this class
	//	can be rendered to the screen by calling the draw
	//	function. Such components will automatically be detected
	//	by the Engine. 
	class EventDriven
	{
	public:
		// Override this method to implement event handling / updating
		virtual void Update(Time dt) = 0;
	};

}
