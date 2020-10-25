#pragma once
#include "COM.hpp"

#include <dinput.h>

namespace orbit
{

	class DXInputResource
	{
	protected:
		// @member: direct input device
		Ptr<IDirectInput8W> _directInput;
	public:
		DXInputResource();
		Ptr<IDirectInput8W> GetDirectInput() const { return _directInput; }
	};

}
