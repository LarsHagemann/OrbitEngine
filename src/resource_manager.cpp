#include "resource_manager.hpp"
#include "exception.hpp"
#include "input_layout.hpp"

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

	Ptr<ID3D12PipelineState> ResourceManager::AddPipelineState(const std::string& id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
	{
		if (_pipelineStates.find(id) != _pipelineStates.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Adding pipeline state."), 10);
		Ptr<ID3D12PipelineState> sPtr;
		ORBIT_THROW_IF_FAILED(GetDevice()->CreateGraphicsPipelineState(
			&desc,
			IID_PPV_ARGS(sPtr.GetAddressOf())),
			"Failed to create pipeline state object"
		);

		_pipelineStates.emplace(id, sPtr);
		return sPtr;
	}

	Ptr<ID3D12PipelineState> ResourceManager::AddPipelineState(
		const std::string& id, 
		const std::string& vertexShaderId, 
		const std::string& pixelShaderId,
		const std::string& rootSignatureId)
	{
		if (_pipelineStates.find(id) != _pipelineStates.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Adding pipeline state."), 10);

		auto vs = GetVertexShader(vertexShaderId);
		auto ps = GetPixelShader(pixelShaderId);

		static const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			CreateInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
			CreateInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT),
			CreateInputElement("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT),
			CreateInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT),
			CreateInputElement("WORLDROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, true),
			CreateInputElement("WORLDROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, true),
			CreateInputElement("WORLDROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, true),
			CreateInputElement("WORLDROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, true),
		};

		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		pipelineStateStream.pRootSignature = GetRootSignature(rootSignatureId).Get();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = GetVertexShader(vertexShaderId)->GetBytecode();
		pipelineStateStream.PS = GetPixelShader(pixelShaderId)->GetBytecode();
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;

		Ptr<ID3D12Device2> device;
		ORBIT_THROW_IF_FAILED(GetDevice()->QueryInterface(device.GetAddressOf()), "Failed to query ID3D12Device2.");

		D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
			sizeof(PipelineStateStream), &pipelineStateStream
		};
		Ptr<ID3D12PipelineState> pso;
		ORBIT_THROW_IF_FAILED(device->CreatePipelineState(
			&pipelineStateStreamDesc,
			IID_PPV_ARGS(pso.GetAddressOf())),
			"Failed to create pipeline state."
		);

		_pipelineStates.emplace(id, pso);
		return pso;
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

	Ptr<ID3D12RootSignature> ResourceManager::AddRootSignature(const std::string& id, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc)
	{
		if (_rootSignatures.find(id) != _rootSignatures.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Adding root signature."), 10);

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (FAILED(GetDevice()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;

		Ptr<ID3DBlob> rootSignatureBlob;
		Ptr<ID3DBlob> errorBlob;
		ORBIT_THROW_IF_FAILED(D3DX12SerializeVersionedRootSignature(
			&desc,
			featureData.HighestVersion, 
			rootSignatureBlob.GetAddressOf(), 
			errorBlob.GetAddressOf()), 
			"Failed to serialize root signature."
		);
		// Create the root signature.
		Ptr<ID3D12RootSignature> rootSignature;
		ORBIT_THROW_IF_FAILED(GetDevice()->CreateRootSignature(
			0, 
			rootSignatureBlob->GetBufferPointer(),
			rootSignatureBlob->GetBufferSize(), 
			IID_PPV_ARGS(&rootSignature)),
			"Failed to create root signature."
		);

		// Append Root Signature
		_rootSignatures.emplace(id, rootSignature);
		return rootSignature;
	}

	Ptr<ID3D12RootSignature> ResourceManager::AddRootSignature(const std::string& id, const fs::path& file)
	{
		if (_rootSignatures.find(id) != _rootSignatures.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Adding root signature."), 10);

		Ptr<ID3DBlob> rsBlob;
		ORBIT_THROW_IF_FAILED(D3DReadFileToBlob(
			file.c_str(),
			rsBlob.GetAddressOf()
		),
			"Failed to read root signature from file."
		);

		Ptr<ID3D12RootSignature> rs;
		ORBIT_THROW_IF_FAILED(GetDevice()->CreateRootSignature(
			0, 
			rsBlob->GetBufferPointer(), 
			rsBlob->GetBufferSize(), 
			IID_PPV_ARGS(rs.GetAddressOf())),
			"Failed to create root signature."
		);
		_rootSignatures.emplace(id, rs);
		return rs;
	}

	Ptr<ID3D12RootSignature> ResourceManager::GetRootSignature(const std::string& id)
	{
		auto rs = _rootSignatures.find(id);
		if (rs != _rootSignatures.end())
			return rs->second;

		return nullptr;
	}

	Ptr<ID3D12RootSignature> ResourceManager::RemoveRootSignature(const std::string& id)
	{
		auto rsIt = _rootSignatures.find(id);
		if (rsIt != _rootSignatures.end())
		{
			auto rs = rsIt->second;
			_rootSignatures.erase(rsIt);
			return rs;
		}

		return nullptr;
	}

	std::shared_ptr<VertexShader> ResourceManager::AddVertexShader(const std::string& id, const fs::path& file)
	{
		if (_vertexShaders.find(id) != _vertexShaders.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Adding vertex shader from file '%s'.", fs::absolute(file).generic_string().c_str()), 10);

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

		ORBIT_INFO_LEVEL(FormatString("Adding pixel shader."), 10);

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