#include "texture.hpp"
#include "dds_texture_loader.hpp"
#include "wic_texture_loader.hpp"
#include "exception.hpp"

namespace orbit
{

	bool Texture::LoadFromFile(const fs::path& filepath, Ptr<ID3D12Device> device)
	{
		ORBIT_INFO_LEVEL(FormatString("Loading texture from file '%s'.", filepath.generic_string().c_str()), 4);
		HRESULT result;
		if (filepath.extension() != ".dds")
		{
			std::unique_ptr<uint8_t[]> data;
			D3D12_SUBRESOURCE_DATA sdata;
			result = LoadWICTextureFromFile(
				device.Get(),
				filepath.c_str(),
				_textureResource.GetAddressOf(),
				data,
				sdata
			);
			if (FAILED(result))
				ORBIT_WARN_HR(FormatString("Failed to load texture '%s'!", filepath.generic_string().data()), result);
		}
		else
		{
			std::unique_ptr<uint8_t[]> data;
			std::vector<D3D12_SUBRESOURCE_DATA> sdata;
			result = LoadDDSTextureFromFile(
				device.Get(),
				filepath.c_str(),
				_textureResource.GetAddressOf(),
				data,
				sdata
			);
			if (FAILED(result))
				ORBIT_WARN_HR(FormatString("Failed to load texture '%s'!", filepath.generic_string().data()), result);
		}
		return SUCCEEDED(result);
	}

	bool Texture::LoadFromBytes(const std::vector<uint8_t>& bytes, Ptr<ID3D12Device> device)
	{
		ORBIT_INFO_LEVEL(FormatString("Loading texture from memory."), 4);
		HRESULT result;
		{
			std::vector<D3D12_SUBRESOURCE_DATA> sdata;
			result = LoadDDSTextureFromMemory(
				device.Get(),
				bytes.data(),
				bytes.size(),
				_textureResource.GetAddressOf(),
				sdata
			);
		}

		if (FAILED(result))
		{
			std::unique_ptr<uint8_t[]> data;
			D3D12_SUBRESOURCE_DATA sdata;
			LoadWICTextureFromMemory(
				device.Get(),
				bytes.data(),
				bytes.size(),
				_textureResource.GetAddressOf(),
				data,
				sdata
			);
		}
		return SUCCEEDED(result);
	}

}