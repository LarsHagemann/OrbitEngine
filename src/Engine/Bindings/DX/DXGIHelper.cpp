#include "Engine/Bindings/DX/DXGIHelper.hpp"
#include "Engine/Misc/Logger.hpp"

#ifdef ORBIT_DX12
#include <d3d12.h>
#else
#include <D3D11.h>
#endif

namespace orbit
{

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
#ifdef ORBIT_DX12
			if ((dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D12CreateDevice(adapter.Get(),
					D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr)) &&
				dxgiAdapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc.DedicatedVideoMemory;
				favored = adapter;
			}
#else // ORBIT_DX11
			D3D_FEATURE_LEVEL fl[] = { D3D_FEATURE_LEVEL_11_0 };
			D3D_FEATURE_LEVEL tfl;
			Ptr<ID3D11Device> device;

			if ((dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0 &&
				SUCCEEDED(D3D11CreateDevice(adapter.Get(),
					D3D_DRIVER_TYPE_REFERENCE,
					nullptr,
					0,
					fl,
					1,
					D3D11_SDK_VERSION,
					device.ReleaseAndGetAddressOf(),
					&tfl,
					nullptr
				)) &&
				dxgiAdapterDesc.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = dxgiAdapterDesc.DedicatedVideoMemory;
				favored = adapter;
			}
#endif
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

}
