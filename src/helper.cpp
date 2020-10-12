#include "helper.hpp"
#include "exception.hpp"

#include <fstream>

namespace orbit
{

	Ptr<ID3DBlob> LoadShaderBinaryFromFile(const fs::path& filepath)
	{
		std::ifstream fin(filepath, std::ios::in | std::ios::binary);
		fin.seekg(0, std::ios_base::end);
		auto fsize = static_cast<int>(fin.tellg());
		fin.seekg(0, std::ios_base::beg);

		Ptr<ID3DBlob> shaderBlob;
		ORBIT_THROW_IF_FAILED(
			D3DCreateBlob(fsize, shaderBlob.ReleaseAndGetAddressOf()),
			"Failed to create shader blob."
		);
		fin.read(static_cast<char*>(shaderBlob->GetBufferPointer()), fsize);
		fin.close();
		return shaderBlob;
	}

	Ptr<ID3DBlob> LoadShaderBinaryFromMemory(std::string_view binary)
	{
		Ptr<ID3DBlob> shaderBlob;
		ORBIT_THROW_IF_FAILED(
			D3DCreateBlob(binary.length(), shaderBlob.ReleaseAndGetAddressOf()),
			"Failed to create shader blob."
		);
		memcpy_s(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), binary.data(), binary.length());
		return shaderBlob;
	}

	Matrix4f PerspectiveFovLH(float vFOV, float aspectRatio, float nearZ, float farZ)
	{
		Matrix4f matrix = Matrix4f::Zero();

		float theta = vFOV * 0.5f;
		float range = farZ - nearZ;
		float invtan = 1.f / tan(theta);
		float tmp = (nearZ * farZ) / range;

		matrix(0, 0) = invtan / aspectRatio;
		matrix(1, 1) = invtan;
		matrix(2, 2) = 2 * tmp;
		matrix(2, 3) = 1;
		matrix(3, 2) = -tmp;

		return matrix;
	}

	Matrix4f LookAt(const Vector3f& eye, const Vector3f& target, const Vector3f& up)
	{
		auto f = (eye - target).normalized();
		auto r = up.cross(f);
		auto u = f.cross(r);
		auto s = f.cross(u).normalized();
		u = s.cross(f);
		
		Matrix4f matrix = Matrix4f::Zero();
		matrix.block<1, 3>(0, 0) = s;
		matrix(0, 3) = -s.dot(eye);
		matrix.block<1, 3>(1, 0) = u;
		matrix(1, 3) = -u.dot(eye);
		matrix.block<1, 3>(2, 0) = -f;
		matrix(2, 3) = f.dot(eye);
		matrix.row(3) << 0, 0, 0, 1;

		return matrix;
	}

	std::vector<Ptr<IDXGIAdapter4>> GetAdapters()
	{
		std::vector<Ptr<IDXGIAdapter4>> adapters;
		Ptr<IDXGIAdapter1> dxgiAdapter1;
		Ptr<IDXGIAdapter4> dxgiAdapter4;
		Ptr<IDXGIFactory4> dxgiFactory;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		ORBIT_THROW_IF_FAILED(
			CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)),
			"Failed to create the DXGI Factory"
		);

		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			ORBIT_THROW_IF_FAILED(
				dxgiAdapter1.As(&dxgiAdapter4),
				"Failed to convert graphics adapter to IDXGIAdapter4."
			);

			adapters.emplace_back(dxgiAdapter4);
		}

		return adapters;
	}

	Ptr<IDXGIAdapter4> GetWARPAdapter()
	{
		Ptr<IDXGIAdapter1> dxgiAdapter1;
		Ptr<IDXGIAdapter4> dxgiAdapter4;
		Ptr<IDXGIFactory4> dxgiFactory;
		UINT createFactoryFlags = 0;
#if defined(_DEBUG)
		createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
		ORBIT_THROW_IF_FAILED(
			CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)),
			"Failed to create the DXGI Factory."
		);

		ORBIT_THROW_IF_FAILED(
			dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(dxgiAdapter1.GetAddressOf())),
			"Failed to retrieve the WARP adapter."
		);
		ORBIT_THROW_IF_FAILED(
			dxgiAdapter1.As(&dxgiAdapter4),
			"Failed to convert the WARP adapter to IDXGIAdapter4."
		);

		return dxgiAdapter4;
	}

	Ptr<IDXGIAdapter4> GetFavoredAdapter()
	{
		return GetFavoredAdapter(GetAdapters());
	}

	Ptr<IDXGIAdapter4> GetFavoredAdapter(const std::vector<Ptr<IDXGIAdapter4>> adapters)
	{
		Ptr<IDXGIAdapter4> favored;

		SIZE_T maxDedicatedVideoMemory = 0;
		for (auto adapter : adapters)
		{
			DXGI_ADAPTER_DESC3 dxgiAdapterDesc;
			adapter->GetDesc3(&dxgiAdapterDesc);

			if ((dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D12CreateDevice(adapter.Get(),
					D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc.DedicatedVideoMemory;
				favored = adapter;
			}
		}

		return favored;
	}

	bool CheckTearingSupport()
	{
		bool allowTearing = false;
		Ptr<IDXGIFactory4> factory4;
		if (SUCCEEDED(CreateDXGIFactory1(IID_PPV_ARGS(&factory4))))
		{
			Ptr<IDXGIFactory5> factory5;
			if (SUCCEEDED(factory4.As(&factory5)))
			{
				if (FAILED(factory5->CheckFeatureSupport(
					DXGI_FEATURE_PRESENT_ALLOW_TEARING,
					&allowTearing, sizeof(allowTearing))))
				{
					allowTearing = false;
				}
			}
		}
		return allowTearing;
	}

	Ptr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Ptr<ID3D12Device> device, 
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags)
	{
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

}