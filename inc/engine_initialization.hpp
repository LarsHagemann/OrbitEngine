#pragma once
#include "helper.hpp"

namespace orbit
{

	// @brief: initialization values for the game engine
	struct EngineDesc
	{
		// @member: the dimensions of the window
		// @default: 1080x720
		Vector2i dimensions;
		// @member: is the window in fullscreen mode?
		//	remember to set the dimensions to a proper fullscreen value
		// @default: false
		bool isFullscreen;
		// @member: the number of backbuffers to the swap chain
		// @default: 3
		unsigned numBackbuffers;
		// @brief: should the engine use the Windows Advanced Rasterization Platform
		//	(WARP) for rendering (using Software Rendering)?
		// @default: false
		bool useWARP;
		// @brief: title of the window
		// @default: "OrbitWindow"
		std::wstring title;
		// @brief: sample count for hardware multisampling
		// @default: 1
		unsigned sampleCount;
		// @brief: sample quality for hardware multisampling
		// @default: 0
		unsigned sampleQuality;
		// @brief: the video adapter (i.e. Graphics card) to use.
		// @default: orbit::GetFavoredAdapter() in helper.hpp
		Ptr<IDXGIAdapter4> adapter;
		// @brief: creates a default engine descriptor
		static EngineDesc DefaultDescriptor();
	};

}
