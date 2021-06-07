#include "Engine/Bindings/DX12/DX12Helper.hpp"
#include "Engine/Misc/Logger.hpp"

#include "Engine/Engine.hpp"

namespace orbit
{

	const char* TranslateFeatureLevel(D3D_FEATURE_LEVEL fLevel)
	{
		switch (fLevel)
		{
		case D3D_FEATURE_LEVEL_1_0_CORE:
			return "-- 1.0 Core --";
			break;
		case D3D_FEATURE_LEVEL_9_1:
			return "-- 9.1 --";
			break;
		case D3D_FEATURE_LEVEL_9_2:
			return "-- 9.2 --";
			break;
		case D3D_FEATURE_LEVEL_9_3:
			return "-- 9.3 --";
			break;
		case D3D_FEATURE_LEVEL_10_0:
			return "-- 10.0 --";
			break;
		case D3D_FEATURE_LEVEL_10_1:
			return "-- 10.1 --";
			break;
		case D3D_FEATURE_LEVEL_11_0:
			return "-- 11.0 --";
			break;
		case D3D_FEATURE_LEVEL_11_1:
			return "-- 11.1 --";
			break;
		case D3D_FEATURE_LEVEL_12_0:
			return "-- 12.0 --";
			break;
		case D3D_FEATURE_LEVEL_12_1:
			return "-- 12.1 --";
			break;
		default:
			return "-- Unknown --";
			break;
		}
	}

	Ptr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Ptr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
		ORBIT_INFO_LEVEL(FormatString("Creating descriptor heap."), 10);

		D3D12_DESCRIPTOR_HEAP_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
		desc.NumDescriptors = numDescriptors;
		desc.Type = type;
		desc.Flags = flags;

		Ptr<ID3D12DescriptorHeap> dHeap;
		ORBIT_THROW_IF_FAILED(device->CreateDescriptorHeap(
			&desc,
			IID_PPV_ARGS(dHeap.GetAddressOf())
		),
			"Failed to create descriptor heap."
		);

		return dHeap;
	}

	void UpdateBufferResource(
		Ptr<ID3D12GraphicsCommandList> commandList,
		Ptr<ID3D12Resource>& resource,
		Ptr<ID3D12Resource>& intermediate,
		size_t elementCount,
		size_t elementByteWidth,
		void* data,
		D3D12_RESOURCE_FLAGS flags)
	{
		auto device = Engine::Get()->GetDevice();
		auto bufferSize = elementCount * elementByteWidth;
		ORBIT_THROW_IF_FAILED(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags),
			D3D12_RESOURCE_STATE_COMMON,
			nullptr,
			IID_PPV_ARGS(resource.ReleaseAndGetAddressOf())), 
			"Failed to create committed resource."
		);
		resource->SetName(L"DEFAULT HEAP BUFFER");

		if (data)
		{
			ORBIT_INFO_LEVEL(FormatString("Updating buffer resource."), 11);

			ORBIT_THROW_IF_FAILED(device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
				D3D12_HEAP_FLAG_NONE,
				&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(intermediate.ReleaseAndGetAddressOf())),
				"Failed to create committed resource."
			);

			D3D12_SUBRESOURCE_DATA subresourceData = {};
			subresourceData.pData = data;
			subresourceData.RowPitch = bufferSize;
			subresourceData.SlicePitch = subresourceData.RowPitch;

			UpdateSubresources(
				commandList.Get(),
				resource.Get(), 
				intermediate.Get(),
				0, 
				0, 
				1, 
				&subresourceData
			);
		}
	}

}
