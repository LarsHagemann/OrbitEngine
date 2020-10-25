#pragma once
#include <unordered_map>
#include "Engine/Bindings/Shader.hpp"
#include "Engine/Rendering/Texture.hpp"
#include "Engine/Rendering/Material.hpp"

namespace orbit
{

	// @brief: manages game resources like textures, materials, etc...
	class ResourceManager
	{
	protected:
		// @member: list of materials used by the engine and the graphics pipeline
		std::unordered_map<std::string, std::shared_ptr<Material>> _materials;
		// @member: list of vertex shaders
		std::unordered_map<std::string, std::shared_ptr<VertexShader>> _vertexShaders;
		// @member: list of pixel shaders
		std::unordered_map<std::string, std::shared_ptr<PixelShader>> _pixelShaders;
		// @member: list of textures
		std::unordered_map<std::string, std::shared_ptr<Texture>> _textures;

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

		// @brief: registers a material to the resource manager
		// @param id: the string id for the material
		// @param material: shared pointer of the material to be registered
		void AddTexture(const std::string& id, std::shared_ptr<Texture> texture);
		// @brief: returns a material from the list
		// @param id: the string id of the material
		// @return: the material with the specific id (or nullptr if not found)
		std::shared_ptr<Texture> GetTexture(const std::string& id);
		// @brief: removes a material from the list
		// @param id: the id of the material to be removed
		// @return: the material that has been removed from the list
		std::shared_ptr<Texture> RemoveTexture(const std::string& id);

		// @method: loads a compiled binary vertex shader from file
		// @param id: the id for the vertex shader
		// @param file: the filepath for the vertex shader
		// @return: shared pointer to the created vertex shader
		//	returns nullptr if the id already exists (or the file cannot be loaded)
		std::shared_ptr<VertexShader> AddVertexShader(const std::string& id, const fs::path& file);
		// @method: returns a vertex shader by id
		// @param id: the id of the vertex shader
		// @return: nullptr if the id did not exist
		std::shared_ptr<VertexShader> GetVertexShader(const std::string& id);
		// @method: removes a vertex shader from the list of vertex shaders
		// @param id: the id of the vertex shader
		// @return: the removed vertex shader or nullptr if non-existent
		std::shared_ptr<VertexShader> RemoveVertexShader(const std::string& id);

		// @method: loads a compiled binary pixel shader from file
		// @param id: the id for the pixel shader
		// @param file: the filepath for the pixel shader
		// @return: shared pointer to the created pixel shader
		//	returns nullptr if the id already exists (or the file cannot be loaded)
		std::shared_ptr<PixelShader> AddPixelShader(const std::string& id, const fs::path& file);
		// @method: returns a pixel shader by id
		// @param id: the id of the pixel shader
		// @return: nullptr if the id did not exist
		std::shared_ptr<PixelShader> GetPixelShader(const std::string& id);
		// @method: removes a pixel shader from the list of pixel shaders
		// @param id: the id of the pixel shader
		// @return: the removed pixel shader or nullptr if non-existent
		std::shared_ptr<PixelShader> RemovePixelShader(const std::string& id);
	};

}