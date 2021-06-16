#include "Engine/Rendering/Material.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Scene.hpp"

namespace orbit
{

	Material::Material(MaterialData initData) :
		_cachedData(initData),
		_buffer(CreateConstantBuffer(initData, L"MaterialBuffer"))
	{
		//memcpy_s(_buffer.CPU, sizeof(MaterialData), &initData, sizeof(MaterialData));
	}

	std::shared_ptr<Material> Material::Create(MaterialData initData)
	{
		return std::make_shared<Material>(initData);
	}

	void Material::BindMaterial(Renderer* renderer, Ptr<ID3D12GraphicsCommandList> cmdList)
	{
		cmdList->SetGraphicsRootConstantBufferView(1, _buffer.view.BufferLocation);
		//renderer->BindConstantBuffer(1, _buffer);
		

		if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_COLOR_MAP))
		{
			renderer->BindTexture(_colorTextureId, SLOT_COLOR_MAP);
		}
		if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_NORMAL_MAP))
		{
			renderer->BindTexture(_normalTextureId, SLOT_NORMAL_MAP);
		}
		if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_ROUGHNESS_MAP))
		{
			renderer->BindTexture(_roughnessTextureId, SLOT_ROUGHNESS_MAP);
		}
		if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_OCCLUSION_MAP))
		{
			renderer->BindTexture(_occlusionTextureId, SLOT_OCCLUSION_MAP);
		}
	}
    
	void Material::SetTexture(TextureType textureType, std::string_view textureId)
	{
		switch (textureType)
		{
		case orbit::TextureType::T_COLOR:     _colorTextureId     = textureId; break;
		case orbit::TextureType::T_NORMAL:    _normalTextureId    = textureId; break;
		case orbit::TextureType::T_OCCLUSION: _occlusionTextureId = textureId; break;
		case orbit::TextureType::T_ROUGHNESS: _roughnessTextureId = textureId; break;
		}
	}

}