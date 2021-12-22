#include "orb/OrbIntermediate.hpp"
#include "orb/OrbFile.hpp"

#include <limits>

namespace orbtool
{

    ResourceType OrbIntermediate::GetObjectType(uint32_t objectIndex) const
    {
		switch (m_objects.at(objectIndex).value.index())
		{
		case 0: return ResourceType::MATERIAL;
		case 1: return ResourceType::MESH;
		case 2: 
			if (GetObject<OrbTexture>(objectIndex).onlyReference)
				return ResourceType::TEXTURE_REFERENCE;
			else
				return ResourceType::TEXTURE;
		case 3: return ResourceType::SHADER_CODE;
		case 4: return ResourceType::INPUT_LAYOUT;
		case 5: return ResourceType::PIPELINE_STATE;
		case 6: return ResourceType::SHADER_BINARY;
		case 7: return ResourceType::RASTERIZER_STATE;
		case 8: return ResourceType::BLEND_STATE;
		}

		return (ResourceType)std::numeric_limits<uint32_t>::max();
    }

	int64_t OrbIntermediate::GetOffsetFromName(const std::string& name, uint64_t offsetId) const
	{
		auto it = m_objectIndices.find(name);
		if (it == m_objectIndices.end())
			return 0;
		
		return it->second - static_cast<int64_t>(offsetId);
	}

}