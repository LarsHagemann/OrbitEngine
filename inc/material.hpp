#pragma once
#include "helper.hpp"

namespace orbit
{

	class Engine;
	using EnginePtr = std::shared_ptr<Engine>;

	// @brief: the type of the texture
	enum class TextureType
	{
		// texture is used as a color texture
		T_COLOR,
		// texture is used as a normal map
		T_NORMAL,
		// texture is used as an occlusion map
		T_OCCLUSION,
		// texture is used as a roughness map
		T_ROUGHNESS
	};

	// @brief: data for creating a material
	struct MaterialData
	{
		// @member: the diffuse color (how the object appears)
		Vector4f diffuse;
		// @member: roughness of the mesh
		float roughness;
		// @member: flags for the material
		uint32_t flags;
		// @unused
		Vector2f padding1;
	};

	enum class TextureFlag
	{
		F_HAS_COLOR_MAP = (1 << 0),
		F_HAS_NORMAL_MAP = (1 << 1),
		F_HAS_OCCLUSION_MAP = (1 << 2),
		F_HAs_ROUGHNESS_MAP = (1 << 3)
	};

	static constexpr auto SLOT_COLOR_MAP = 0u;
	static constexpr auto SLOT_NORMAL_MAP = 1u;
	static constexpr auto SLOT_OCCLUSION_MAP = 2u;
	static constexpr auto SLOT_ROUGHNESS_MAP = 3u;

	// @brief: makes sending material data to the graphics card easy
	//	and gives a level of abstraction
	class Material
	{
	protected:
		// @member: shared pointer of the game engine
		EnginePtr _engine;
		// @member: the cached material data
		MaterialData _cachedData;

		// @member: id of the color texture (might be empty)
		std::string _colorTextureId;
		// @member: id of the normal texture (might be empty)
		std::string _normalTextureId;
		// @member: id of the occlusion texture (might be empty)
		std::string _occlusionTextureId;
		// @member: id of the roughness texture (might be empty)
		std::string _roughnessTextureId;
	protected:
		// @method: reloads the buffer whenever _cachedData has been changed
		void ReloadBuffer();
	public:
		// @method: creates a new material initialized with initData
		// @param engine: shared pointer to the game engine
		// @param initData: data to initialize the material with
		// @note: use Material::create(...) instead
		Material(EnginePtr engine, MaterialData initData);
		// @method: creates a new material initialized with initData
		// @param engine: shared pointer to the game engine
		// @param initData: data to initialize the material with
		static std::shared_ptr<Material> create(EnginePtr engine, MaterialData initData);
		Material(const Material& other) = default;
		Material(Material&& other) = default;
		Material& operator=(const Material& other) = default;
		Material& operator=(Material&& other) = default;

		// @method: binds the material to the graphics pipeline
		void BindMaterial();
		// @method: sets the texture for a specific slot
		void SetTexture(TextureType textureType, std::string_view textureId);
	};

}
