#include "resource_manager.hpp"

namespace orbit
{

	void ResourceManager::AddMaterial(const std::string& id, std::shared_ptr<Material> material)
	{
#ifdef _DEBUG
		if (_materials.find(id) != _materials.end())
			return; // id is already present. Skip.
#endif
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

	void ResourceManager::AddPipelineState(const std::string& id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
	{
#ifdef _DEBUG
		if (_pipelineStates.find(id) != _pipelineStates.end())
			return;
#endif
		Ptr<ID3D12PipelineState> sPtr;
		GetDevice()->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(sPtr.GetAddressOf())
		);

		_pipelineStates.emplace(id, sPtr);
	}

	void ResourceManager::AddPipelineState(const std::string& id, std::string_view vertexShaderId, std::string_view pixelShaderId)
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));


	}

	Ptr<ID3D12PipelineState> ResourceManager::GetPipelineState(const std::string& id)
	{
		auto it = _pipelineStates.find(id);
		if (it == _pipelineStates.end()) return nullptr;

		return it->second;
	}

	Ptr<ID3D12PipelineState> ResourceManager::RemovePipelineState(const std::string& id)
	{
		auto it = _pipelineStates.find(id);
		if (it == _pipelineStates.end()) return nullptr;

		auto ptr = it->second;
		_pipelineStates.erase(it);
		return ptr;
	}

}