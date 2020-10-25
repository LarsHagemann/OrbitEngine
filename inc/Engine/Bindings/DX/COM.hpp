#pragma once
#include <wrl/client.h>

namespace orbit
{
	namespace wrl = Microsoft::WRL;

	template<class COM>
	using Ptr = wrl::ComPtr<COM>;

}