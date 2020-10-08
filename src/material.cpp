#include "material.hpp"
#include "engine.hpp"

namespace orbit
{

	void Material::ReloadBuffer()
	{
		
	}

	Material::Material(EnginePtr engine, MaterialData initData) :
		_cachedData(initData),
		_engine(engine)
	{
		ReloadBuffer();
	}

	std::shared_ptr<Material> Material::create(EnginePtr engine, MaterialData initData)
	{
		return std::make_shared<Material>(engine, initData);
	}

	void Material::BindMaterial()
	{
	}

	void Material::SetTexture(TextureType textureType, std::string_view textureId)
	{
		switch (textureType)
		{
		case orbit::TextureType::T_COLOR: _colorTextureId = textureId; break;
		case orbit::TextureType::T_NORMAL: _normalTextureId = textureId; break;
		case orbit::TextureType::T_OCCLUSION: _occlusionTextureId = textureId; break;
		case orbit::TextureType::T_ROUGHNESS: _roughnessTextureId = textureId; break;
		}
	}

}