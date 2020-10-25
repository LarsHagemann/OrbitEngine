#pragma once
#include "COM.hpp"
#include <dxgi1_6.h>
#include <vector>

namespace orbit
{

	extern Ptr<IDXGIAdapter4> GetWARPAdapter();
	extern std::vector<Ptr<IDXGIAdapter4>> GetAdapters();
	extern Ptr<IDXGIAdapter4> GetFavoredAdapter();
	extern Ptr<IDXGIAdapter4> GetFavoredAdapter(const std::vector<Ptr<IDXGIAdapter4>> adapters);
	extern bool CheckTearingSupport();

}
