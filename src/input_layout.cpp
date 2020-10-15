#include "input_layout.hpp"

namespace orbit
{

	D3D12_INPUT_ELEMENT_DESC CreateInputElement(std::string_view name, unsigned index, DXGI_FORMAT format, bool instanceData, unsigned instanceDataStepRate)
	{
		return {
			name.data(),
			index,
			format,
			0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			instanceData ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			instanceDataStepRate
		};
	}

}