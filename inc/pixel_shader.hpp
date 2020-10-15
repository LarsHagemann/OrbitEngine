#pragma once
#include "helper.hpp"
#include <d3dcompiler.h>

namespace orbit
{

	class PixelShader
	{
	private:
		D3D12_SHADER_BYTECODE _bytecode;
	public:
		~PixelShader();
		// @method: loads a pixel shader binary
		//	the binary must have been compiled (for example with fxc.exe)
		// @param filepath: the path to the compiled shader binary
		void LoadBinary(const fs::path& filepath);
		// @method: returns the internal bytecode
		D3D12_SHADER_BYTECODE GetBytecode() const { return _bytecode; }
	};

}
