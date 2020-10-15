#include "engine_initialization.hpp"

namespace orbit
{

	EngineDesc EngineDesc::DefaultDescriptor()
	{
		EngineDesc desc;
		desc.dimensions = Vector2i{ 1080, 720 };
		desc.isFullscreen = false;
		desc.numBackbuffers = 3;
		desc.useWARP = false;
		desc.title = L"OrbitWindow";
		desc.adapter = GetFavoredAdapter();
		desc.sampleCount = 1;
		desc.sampleQuality = 0;
		desc.clearColor = Vector4f{ 1.f, 1.f, 1.f, 1.f };
		return desc;
	}

}