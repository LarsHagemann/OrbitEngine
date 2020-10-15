#pragma once
#include "helper.hpp"

namespace orbit
{

	class Texture
	{
	protected:
		// @member: buffer for storing texture data
		Ptr<ID3D12Resource> _textureResource;
	public:
		// @method: returns the texture buffer
		Ptr<ID3D12Resource> GetResource() const { return _textureResource; }
		// @method: loads a texture from file
		// @param filepath: file to be loaded
		// @param device: dx12 device used for loading
		// @return: returns true if texture loading was successful
		bool LoadFromFile(const fs::path& filepath, Ptr<ID3D12Device> device);
		// @method: loads a texture from bytes
		// @param bytes: the bytes to be interpreted as a texture
		// @param device: dx12 device used for loading
		// @return: returns true if texture loading was successful
		bool LoadFromBytes(const std::vector<uint8_t>& bytes, Ptr<ID3D12Device> device);
	};

}
