#include "Engine/Rendering/Material.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Scene.hpp"

namespace orbit
{

	Material::Material(MaterialData initData) :
		_cachedData(initData)
	{
		// This buffer is probably static, thus, put it in the default heap
	}

	std::shared_ptr<Material> Material::Create(MaterialData initData)
	{
		return std::make_shared<Material>(initData);
	}

	//void Material::BindMaterial()
	//{
	//	D3D12_GPU_DESCRIPTOR_HANDLE mbHandle;
//
	//	CD3DX12_GPU_DESCRIPTOR_HANDLE cmbHandle;
	//	cmbHandle.InitOffsetted(mbHandle, 1);
	//	cmdList->SetGraphicsRootDescriptorTable(0, cmbHandle);
	//	if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_COLOR_MAP))
	//	{
	//		_engine->BindTexture(SLOT_COLOR_MAP, _colorTextureId, cmdList);
	//	}
	//	if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_NORMAL_MAP))
	//	{
	//		_engine->BindTexture(SLOT_NORMAL_MAP, _normalTextureId, cmdList);
	//	}
	//	if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_ROUGHNESS_MAP))
	//	{
	//		_engine->BindTexture(SLOT_ROUGHNESS_MAP, _roughnessTextureId, cmdList);
	//	}
	//	if (_cachedData.IsFlagSet(MaterialFlag::F_HAS_OCCLUSION_MAP))
	//	{
	//		_engine->BindTexture(SLOT_OCCLUSION_MAP, _occlusionTextureId, cmdList);
	//	}
	//}
    
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