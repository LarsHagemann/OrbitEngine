#pragma once
#include "helper.hpp"

namespace orbit
{

	// @brief: creates a D3D12_INPUT_ELEMENT_DESC
	// @param name: SemanticName for the element
	// @param index: SemanticIndex for the element
	// @param format: the dxgi format of the element
	// @param instanceData: is the element part of the per instance data 
	//	or of the per vertex data
	// @param instanceDataStepRate: width of per instance data
	// @return: the created D3D12_INPUT_ELEMENT_DESC
	extern D3D12_INPUT_ELEMENT_DESC CreateInputElement(
		std::string_view name,
		unsigned index,
		DXGI_FORMAT format,
		bool instanceData = false,
		unsigned instanceDataStepRate = 0u
	);

}
