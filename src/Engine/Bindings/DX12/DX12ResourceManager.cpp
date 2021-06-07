#include "Engine/Bindings/DX12/DX12ResourceManager.hpp"
#include "Engine/Bindings/DX12/DX12Helper.hpp"

namespace orbit
{

    Ptr<ID3D12PipelineState> DX12ResourceManager::AddPipelineState(const std::string& id, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc)
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

	Ptr<ID3D12PipelineState> DX12ResourceManager::AddPipelineState(
		const std::string& id, 
		const std::string& vertexShaderId, 
		const std::string& pixelShaderId)
	{
		if (_pipelineStates.find(id) != _pipelineStates.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Adding pipeline state %s", id.c_str()), 10);

		auto vs = GetVertexShader(vertexShaderId);
		auto ps = GetPixelShader(pixelShaderId);

		static const D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			CreateInputElement("POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT),
			CreateInputElement("NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT),
			CreateInputElement("TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT),
			CreateInputElement("TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT),
			CreateInputElement("WORLDROW", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, true),
			CreateInputElement("WORLDROW", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, true),
			CreateInputElement("WORLDROW", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, true),
			CreateInputElement("WORLDROW", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, true),
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
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER rasterizer;
		} pipelineStateStream;

		D3D12_RT_FORMAT_ARRAY rtvFormats = {};
		rtvFormats.NumRenderTargets = 1;
		rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

		CD3DX12_RASTERIZER_DESC rDesc;
		ZeroMemory(&rDesc, sizeof(D3D12_RASTERIZER_DESC));
		rDesc.AntialiasedLineEnable = true;
		rDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		rDesc.CullMode = D3D12_CULL_MODE_BACK;
		rDesc.DepthBias = 0;
		rDesc.DepthBiasClamp = 0.f;
		rDesc.DepthClipEnable = false;
		rDesc.FillMode = D3D12_FILL_MODE_SOLID;
		rDesc.ForcedSampleCount = 0;
		rDesc.FrontCounterClockwise = true;
		rDesc.MultisampleEnable = false;
		rDesc.SlopeScaledDepthBias = 0.f;

		// Expect root signature of the same id to exist:
		auto rs = GetRootSignature(id);

		pipelineStateStream.pRootSignature = rs.Get();
		pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = GetVertexShader(vertexShaderId)->GetBytecode();
		pipelineStateStream.PS = GetPixelShader(pixelShaderId)->GetBytecode();
		pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		pipelineStateStream.RTVFormats = rtvFormats;
		pipelineStateStream.rasterizer = rDesc;

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

	Ptr<ID3D12PipelineState> DX12ResourceManager::GetPipelineState(const std::string& id)
	{
		auto it = _pipelineStates.find(id);
		if (it == _pipelineStates.end()) return nullptr;

		return it->second;
	}

	Ptr<ID3D12PipelineState> DX12ResourceManager::RemovePipelineState(const std::string& id)
	{
		auto it = _pipelineStates.find(id);
		if (it == _pipelineStates.end()) return nullptr;

		auto ptr = it->second;
		_pipelineStates.erase(it);
		return ptr;
	}

	Ptr<ID3D12RootSignature> DX12ResourceManager::AddRootSignature(const std::string& id, const D3D12_VERSIONED_ROOT_SIGNATURE_DESC& desc)
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
		if (FAILED(D3DX12SerializeVersionedRootSignature(
			&desc,
			featureData.HighestVersion,
			rootSignatureBlob.GetAddressOf(),
			errorBlob.GetAddressOf())))
		{
			std::string buffer;
			buffer.resize(errorBlob->GetBufferSize());
			memcpy_s(buffer.data(), errorBlob->GetBufferSize(), errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
			ORBIT_THROW(FormatString("Failed to serialize root signature: %s", buffer.c_str()));
		}
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

	Ptr<ID3D12RootSignature> DX12ResourceManager::AddRootSignature(const std::string& id, const fs::path& file)
	{
		if (_rootSignatures.find(id) != _rootSignatures.end())
			return nullptr;

		ORBIT_INFO_LEVEL(FormatString("Loading root signature from file '%s'.", fs::absolute(file).generic_string().c_str()), 10);

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

	Ptr<ID3D12RootSignature> DX12ResourceManager::GetRootSignature(const std::string& id)
	{
		auto rs = _rootSignatures.find(id);
		if (rs != _rootSignatures.end())
			return rs->second;

		return nullptr;
	}

	Ptr<ID3D12RootSignature> DX12ResourceManager::RemoveRootSignature(const std::string& id)
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

}