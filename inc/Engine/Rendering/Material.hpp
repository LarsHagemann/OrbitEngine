#pragma once
#include "Engine/Misc/Helper.hpp"

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

	enum class MaterialFlag
	{
		F_HAS_COLOR_MAP = (1 << 0),
		F_HAS_NORMAL_MAP = (1 << 1),
		F_HAS_OCCLUSION_MAP = (1 << 2),
		F_HAS_ROUGHNESS_MAP = (1 << 3)
	};

	// @brief: data for creating a material
	struct MaterialData
	{
		// @member: the diffuse color (how the object appears)
		Vector4f diffuse = Vector4f::Ones();
		// @member: roughness of the mesh
		float roughness = 0.f;
		// @member: flags for the material
		uint32_t flags = 0;
		// @unused
		Vector2f padding1 = Vector2f::Zero();

		// @method: returns whether a certain flag is set
		// @param flag: flag to be tested
		bool IsFlagSet(MaterialFlag flag)
		{
			return ((unsigned)flags & (unsigned)flag) == (unsigned)flag;
		}
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
	public:
		// @method: creates a new material initialized with initData
		// @param engine: shared pointer to the game engine
		// @param initData: data to initialize the material with
		// @note: use Material::create(...) instead
		Material(MaterialData initData);
		// @method: creates a new material initialized with initData
		// @param engine: shared pointer to the game engine
		// @param initData: data to initialize the material with
		static std::shared_ptr<Material> Create(MaterialData initData);
		Material(const Material& other) = default;
		Material(Material&& other) = default;
		Material& operator=(const Material& other) = default;
		Material& operator=(Material&& other) = default;

		// @method: sets the texture for a specific slot
		void SetTexture(TextureType textureType, std::string_view textureId);
	};

}