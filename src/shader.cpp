#include "shader.hpp"
#include "exception.hpp"

namespace orbit
{
	Shader::~Shader()
	{
		_bytecode.BytecodeLength = 0;
		if (_bytecode.pShaderBytecode)
			delete _bytecode.pShaderBytecode;
	}

	void Shader::LoadBinary(const fs::path& filepath)
	{
		Ptr<ID3DBlob> shaderBlob;
		ORBIT_THROW_IF_FAILED(D3DReadFileToBlob(
			filepath.c_str(),
			shaderBlob.GetAddressOf()
		),
			FormatString("Failed to load shader from file %s.", filepath.generic_string().c_str())
		);

		_bytecode = CD3DX12_SHADER_BYTECODE(shaderBlob.Get());
	}

}