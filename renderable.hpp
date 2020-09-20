#pragma once
#include "helper.hpp"

namespace orbit
{

	// @brief: Components inheriting this class
	//	can be rendered to the screen by calling the draw
	//	function. Such components will automatically be detected
	//	by the Engine. 
	class Renderable
	{
	public:
		// Override this method to implement rendering
		virtual void Draw(Ptr<ID3D12CommandList> renderTo) const = 0;
	};

}
