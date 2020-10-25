#include "Engine/Bindings/DX/DXInput.hpp"
#include "Engine/Misc/Logger.hpp"

namespace orbit
{

	DXInputResource::DXInputResource()
	{
		auto hInstance = GetModuleHandle(nullptr);

		ORBIT_THROW_IF_FAILED(DirectInput8Create(
			hInstance,
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&_directInput,
			0
		), "Failed to create directX input device");
	}

}