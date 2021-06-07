#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/DX/DXGIHelper.hpp"

#include "Engine/Misc/Helper.hpp"
#include "Engine/Misc/Logger.hpp"

#include <d3dx12.h>
#include <d3d12.h>

namespace orbit
{

	extern const char* TranslateFeatureLevel(D3D_FEATURE_LEVEL fLevel);

	extern Ptr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Ptr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

	extern void UpdateBufferResource(
		Ptr<ID3D12GraphicsCommandList> commandList,
		Ptr<ID3D12Resource>& resource,
		Ptr<ID3D12Resource>& intermediate,
		size_t elementCount,
		size_t elementByteWidth,
		void* data,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
	);

	// @brief: creates a D3D12_INPUT_ELEMENT_DESC
	// @param name: SemanticName for the element
	// @param index: SemanticIndex for the element
	// @param format: the dxgi format of the element
	// @param instanceData: is the element part of the per instance data 
	//	or of the per vertex data
	// @param instanceDataStepRate: width of per instance data
	// @return: the created D3D12_INPUT_ELEMENT_DESC
	static D3D12_INPUT_ELEMENT_DESC CreateInputElement(
		std::string_view name,
		unsigned index,
		DXGI_FORMAT format,
		unsigned inputSlot = 0u,
		bool instanceData = false,
		unsigned instanceDataStepRate = 0u)
	{
		D3D12_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = name.data();
		desc.SemanticIndex = index;
		desc.Format = format;
		desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		desc.InputSlotClass = instanceData ? D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA : D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = instanceDataStepRate;
		desc.InputSlot = inputSlot;
		return desc;
	}

}
