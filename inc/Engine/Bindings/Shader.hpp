#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Misc/Helper.hpp"
#include "Engine/Misc/Logger.hpp"

namespace orbit
{

#ifdef ORBIT_DX12
class Shader
	{
	protected:
		// @member: bytecode wrapper for pso creation
		D3D12_SHADER_BYTECODE _bytecode;
		// @member: this method actually holds the shader bytecode
		Ptr<ID3DBlob> _shaderBlob;
	public:
		// @method: loads a pixel shader binary
		//	the binary must have been compiled (for example with fxc.exe)
		// @param filepath: the path to the compiled shader binary
		void LoadBinary(const fs::path& filepath)
		{
			ORBIT_THROW_IF_FAILED(D3DReadFileToBlob(
				filepath.c_str(),
				_shaderBlob.GetAddressOf()),
				FormatString("Failed to load shader from file %s.", filepath.generic_string().c_str())
			);
			_bytecode = CD3DX12_SHADER_BYTECODE(_shaderBlob.Get());
		}
		// @method: returns the internal bytecode
		D3D12_SHADER_BYTECODE GetBytecode() const { return _bytecode; }
	};

	using VertexShader = Shader;
	using PixelShader = Shader;
#endif

}
