#include "helper.hpp"
#include "exception.hpp"

#include <fstream>

namespace orbit
{

	Ptr<ID3DBlob> LoadShaderBinaryFromFile(const fs::path& filepath)
	{
		std::ifstream fin(filepath, std::ios::in | std::ios::binary);
		fin.seekg(0, std::ios_base::end);
		auto fsize = static_cast<int>(fin.tellg());
		fin.seekg(0, std::ios_base::beg);

		Ptr<ID3DBlob> shaderBlob;
		ORBIT_THROW_IF_FAILED(
			D3DCreateBlob(fsize, shaderBlob.ReleaseAndGetAddressOf()),
			"Failed to create shader blob."
		);
		fin.read(static_cast<char*>(shaderBlob->GetBufferPointer()), fsize);
		fin.close();
		return shaderBlob;
	}

	Ptr<ID3DBlob> LoadShaderBinaryFromMemory(std::string_view binary)
	{
		Ptr<ID3DBlob> shaderBlob;
		ORBIT_THROW_IF_FAILED(
			D3DCreateBlob(binary.length(), shaderBlob.ReleaseAndGetAddressOf()),
			"Failed to create shader blob."
		);
		memcpy_s(shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize(), binary.data(), binary.length());
		return shaderBlob;
	}

	Matrix4f PerspectiveFovLH(float vFOV, float aspectRatio, float nearZ, float farZ)
	{
		Matrix4f matrix = Matrix4f::Zero();

		float theta = vFOV * 0.5f;
		float range = farZ - nearZ;
		float invtan = 1.f / tan(theta);
		float tmp = (nearZ * farZ) / range;

		matrix(0, 0) = invtan / aspectRatio;
		matrix(1, 1) = invtan;
		matrix(2, 2) = 2 * tmp;
		matrix(2, 3) = 1;
		matrix(3, 2) = -tmp;

		return matrix;
	}

	Matrix4f LookAt(const Vector3f& eye, const Vector3f& target, const Vector3f& up)
	{
		auto f = (eye - target).normalized();
		auto r = up.cross(f);
		auto u = f.cross(r);
		auto s = f.cross(u).normalized();
		u = s.cross(f);
		
		Matrix4f matrix = Matrix4f::Zero();
		matrix.block<1, 3>(0, 0) = s;
		matrix(0, 3) = -s.dot(eye);
		matrix.block<1, 3>(1, 0) = u;
		matrix(1, 3) = -u.dot(eye);
		matrix.block<1, 3>(2, 0) = -f;
		matrix(2, 3) = f.dot(eye);
		matrix.row(3) << 0, 0, 0, 1;

		return matrix;
	}

}