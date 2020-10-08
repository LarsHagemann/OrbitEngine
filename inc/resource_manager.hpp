#pragma once
#include <unordered_map>
#include "material.hpp"

namespace orbit
{

	// @brief: manages game resources like textures, materials, etc...
	class ResourceManager
	{
	protected:
		// @member: list of materials used by the engine and the graphics pipeline
		std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
		// @member: list of pipeline state objects
		std::unordered_map<std::string, Ptr<ID3D12PipelineState>> _pipelineStates;

		// @brief: helper function
		virtual Ptr<ID3D12Device> GetDevice() const = 0;
	public:
		// @brief: registers a material to the resource manager
		// @param id: the string id for the material
		// @param material: shared pointer of the material to be registered
		void AddMaterial(const std::string& id, std::shared_ptr<Material> material);
		// @brief: returns a material from the list
		// @param id: the string id of the material
		// @return: the material with the specific id (or nullptr if not found)
		std::shared_ptr<Material> GetMaterial(const std::string& id);
		// @brief: removes a material from the list
		// @param id: the id of the material to be removed
		// @return: the material that has been removed from the list
		std::shared_ptr<Material> RemoveMaterial(const std::string& id);

		// @brief: registers a pipeline state to the resource manager
		// @param id: the string id for the pipeline state
		// @param desc: DX12 desc object for the pipeline state
		void AddPipelineState(const std::string& id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc);
		// @brief: registers a pipeline state to the resource manager
		// @param id: the string id for the pipeline state
		// @param vertexShaderId: the string id for the vertex shader to be used
		// @param pixelShaderId: the string id for the pixel shader to be used
		void AddPipelineState(
			const std::string& id,
			std::string_view vertexShaderId,
			std::string_view pixelShaderId
		);
		// @brief: returns a pipeline state from the list
		// @param id: the string id of the pipeline state
		// @return: the pipeline state with the specific id (or nullptr if not found)
		Ptr<ID3D12PipelineState> GetPipelineState(const std::string& id);
		// @brief: removes a pipeline state from the list
		// @param id: the id of the pipeline state to be removed
		// @return: the pipeline state that has been removed from the list
		Ptr<ID3D12PipelineState> RemovePipelineState(const std::string& id);
	};

}
