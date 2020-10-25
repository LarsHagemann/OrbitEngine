#pragma once
#include "Component.hpp"

namespace orbit
{

	struct RenderState;

	class Renderable
	{
	public:
		friend class Renderer;
		virtual void Draw(Renderer* renderer) const = 0;
	};

}
