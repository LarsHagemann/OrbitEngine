#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Misc/ResourceManager.hpp"

#include <d3d12.h>

namespace orbit
{

    class DX12ResourceManager : public ResourceManager
    {
    protected:
        // @member: list of pipeline state objects
		std::unordered_map<std::string, Ptr<ID3D12PipelineState>> _pipelineStates;
		// @member: list of root signatures
		std::unordered_map<std::string, Ptr<ID3D12RootSignature>> _rootSignatures;
    public:
        // @brief: registers a pipeline state to the resource manager
		// @param id: the string id for the pipeline state
		// @param desc: DX12 desc object for the pipeline state
		Ptr<ID3D12PipelineState> AddPipelineState(const std::string& id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
		// @brief: registers a pipeline state to the resource manager
		// @param id: the string id for the pipeline state
		// @param vertexShaderId: the string id for the vertex shader to be used
		// @param pixelShaderId: the string id for the pixel shader to be used
		Ptr<ID3D12PipelineState> AddPipelineState(
			const std::string& id,
			const std::string& vertexShaderId,
			const std::string& pixelShaderId
		);
		// @brief: returns a pipeline state from the list
		// @param id: the string id of the pipeline state
		// @return: the pipeline state with the specific id (or nullptr if not found)
		Ptr<ID3D12PipelineState> GetPipelineState(const std::string& id);
		// @brief: removes a pipeline state from the list
		// @param id: the id of the pipeline state to be removed
		// @return: the pipeline state that has been removed from the list
		Ptr<ID3D12PipelineState> RemovePipelineState(const std::string& id);

		// @method: registers a root signature to the resource manager
		// @param id: the string id for the root signature
		// @param desc: DX12 desc object for the root signature
		Ptr<ID3D12RootSignature> AddRootSignature(const std::string& id, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc);
		// @method: registers a root signature to the resource manager
		// @param id: the string id for the root signature
		// @param file: the compiled root signature object to load
		// @return: the created root signature
		Ptr<ID3D12RootSignature> AddRootSignature(const std::string& id, const fs::path& file);
		// @method: returns a root signature from the list
		// @param id: the string id of the root signature
		// @return: the root signature with the specific id (or nullptr if not found)
		Ptr<ID3D12RootSignature> GetRootSignature(const std::string& id);
		// @method: removes a root signature from the list
		// @param id: the id of the root signature to be removed
		// @return: the root signature that has been removed from the list
		Ptr<ID3D12RootSignature> RemoveRootSignature(const std::string& id);
    };

}
