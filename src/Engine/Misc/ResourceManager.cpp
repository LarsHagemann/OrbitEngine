#include "Engine/Misc/ResourceManager.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Bindings/Binding.hpp"

#include <iostream>

namespace orbit
{

	void ResourceManager::AddMaterial(const std::string& id, std::shared_ptr<Material> material)
	{
		if (_materials.find(id) != _materials.end())
			return; // id is already present. Skip.

		ORBIT_INFO_LEVEL(FormatString("Adding material."), 10);
		_materials.emplace(id, material);
	}

	std::shared_ptr<Material> ResourceManager::GetMaterial(const std::string& id)
	{
		auto it = _materials.find(id);
		if (it == _materials.end()) return nullptr;

		return it->second;
	}

	std::shared_ptr<Material> ResourceManager::RemoveMaterial(const std::string& id)
	{
		auto it = _materials.find(id);
		if (it == _materials.end()) return nullptr;

		auto ptr = it->second;
		_materials.erase(it);
		return ptr;
	}

	void ResourceManager::AddTexture(const std::string& id, std::shared_ptr<Texture> texture)
	{
		if (_materials.find(id) != _materials.end())
			return; // id is already present. Skip.

		ORBIT_INFO_LEVEL(FormatString("Adding texture."), 10);
		_textures.emplace(id, texture);
	}

	std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string& id)
	{
		auto it = _textures.find(id);
		if (it == _textures.end()) return nullptr;

		return it->second;
	}

	std::shared_ptr<Texture> ResourceManager::RemoveTexture(const std::string& id)
	{
		auto it = _textures.find(id);
		if (it == _textures.end()) return nullptr;

		auto ptr = it->second;
		_textures.erase(it);
		return ptr;
	}

	std::shared_ptr<VertexShader> ResourceManager::AddVertexShader(const std::string& id, const fs::path& file)
	{
		if (_vertexShaders.find(id) != _vertexShaders.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Loading vertex shader from file '%s'.", fs::absolute(file).generic_string().c_str()), 10);

		auto vs = std::make_shared<VertexShader>();
		vs->LoadBinary(file);
		_vertexShaders.emplace(id, vs);
		return vs;
	}

	std::shared_ptr<VertexShader> ResourceManager::GetVertexShader(const std::string& id)
	{

		auto it = _vertexShaders.find(id);
		if (it != _vertexShaders.end())
			return it->second;

		return nullptr;
	}

	std::shared_ptr<VertexShader> ResourceManager::RemoveVertexShader(const std::string& id)
	{
		auto vsIt = _vertexShaders.find(id);
		if (vsIt != _vertexShaders.end())
		{
			auto vs = vsIt->second;
			_vertexShaders.erase(vsIt);
			return vs;
		}

		return nullptr;
	}

	std::shared_ptr<PixelShader> ResourceManager::AddPixelShader(const std::string& id, const fs::path& file)
	{
		if (_pixelShaders.find(id) != _pixelShaders.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Loading pixel shader from file '%s'.", fs::absolute(file).generic_string().c_str()), 10);

		auto ps = std::make_shared<PixelShader>();
		ps->LoadBinary(file);
		_pixelShaders.emplace(id, ps);
		return ps;
	}

	std::shared_ptr<PixelShader> ResourceManager::GetPixelShader(const std::string& id)
	{
		auto it = _pixelShaders.find(id);
		if (it != _pixelShaders.end())
			return it->second;

		return nullptr;
	}

	std::shared_ptr<PixelShader> ResourceManager::RemovePixelShader(const std::string& id)
	{
		auto psIt = _pixelShaders.find(id);
		if (psIt != _pixelShaders.end())
		{
			auto ps = psIt->second;
			_pixelShaders.erase(psIt);
			return ps;
		}

		return nullptr;
	}

}