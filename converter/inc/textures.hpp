#pragma once
#include <cstdint>

namespace tex
{
	
	enum class MaterialFlag
	{
		F_HAS_COLOR_MAP = (1 << 0),
		F_HAS_NORMAL_MAP = (1 << 1),
		F_HAS_OCCLUSION_MAP = (1 << 2),
		F_HAS_ROUGHNESS_MAP = (1 << 3)
	};

	static constexpr uint8_t TEXTURE_COLOR = 0;
	static constexpr uint8_t TEXTURE_NORMAL = 1;
	static constexpr uint8_t TEXTURE_OCCLUSION = 2;
	static constexpr uint8_t TEXTURE_ROUGHNESS = 3;
	static constexpr uint8_t TEXTURE_UNKNOWN = 0b1000'0000;

}
