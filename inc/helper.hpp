#pragma once
#include "PxConfig.h"
#include <filesystem>

#include <wrl/client.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <dinput.h>
#include <DirectXMath.h>

#include "Eigen/Dense"

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)

namespace orbit
{

	namespace fs = std::filesystem;
	namespace wrl = Microsoft::WRL;

	using namespace Eigen;
	using namespace DirectX;

	template<class COM>
	using Ptr = wrl::ComPtr<COM>;

	// @brief: Constant buffers need to be uploaded to the graphics card in
	//	256 byte chunks. This function will calculate the chunk size for
	//	a struct T that should be uploaded.
	//	Let T be the struct you want to send to the graphics card, then you need
	//	a buffer of size CalcConstantBufferSize<T>() to do that.
	template<typename T>
	constexpr size_t CalcConstantBufferSize()
	{
		return (sizeof(T) + 255) & ~255;
	}

	// @brief: This function loads a compiled shader binary file into a blob
	//	the shader must previously be compiled using fxc (or a similar tool).
	//	You cannot simply load the shader source code into memory (I mean, you can, but it doesn't do much)
	// @param filepath: the filepath to the compiled binary
	// @return: the shader binary data
	// @throws: this function throws an std::exception if it failed to load the binary
	//	or the blob creation failed
	extern Ptr<ID3DBlob> LoadShaderBinaryFromFile(const fs::path& filepath);

	// @brief: This function loads a compiled shader binary from memory into a blob
	//	the shader must previously be compiled using fxc (or a similar tool).
	//	You cannot simply load the shader source code into memory (I mean, you can, but it doesn't do much)
	// @param binary: the shader binary data
	// @return: the shader binary data as a blob
	// @throws: this function throws an std::exception if it failed to load the binary
	extern Ptr<ID3DBlob> LoadShaderBinaryFromMemory(std::string_view binary);

	// @brief: returns a formatted string
	// @template Size: the size of the internal array passed to sprintf_s
	// @template ...Args: types of the format arguments
	// @param format: the format string
	// @param ...args: the args to the format string
	// @return: the formatted string
	template<size_t Size = 1024, typename ...Args>
	std::string FormatString(std::string_view format, Args... args)
	{
		char str[Size];
		auto size = sprintf_s(str, format.data(), args...);
		return std::string(str, size);
	}

	// @brief: calculates a field-of-view left handed projection matrix
	// @param vFOV: the vertical field of view in radians
	// @param aspectRatio: the screens aspect ratio (screenwidth/screenheight)
	// @param nearZ: the distance of the near plane
	// @param farZ: the distance of the far plane
	// @note: for best results (farZ - nearZ) should be kept as low as possible
	extern Matrix4f PerspectiveFovLH(float vFOV, float aspectRatio, float nearZ, float farZ);

	// @brief: calculates a view matrix looking from eye to target
	// @param eye: the eye position of the camera
	// @param target: the target to look at
	// @param up: camera up vector (most likely [0,1,0])
	// @return: the view matrix
	extern Matrix4f LookAt(const Vector3f& eye, const Vector3f& target, const Vector3f& up);

	// @brief: enumerates a list of all the graphics adapters
	//	(hardware and software)
	// @return: list of all the video adapters (graphics cards and software adapters)
	extern std::vector<Ptr<IDXGIAdapter4>> GetAdapters();

	// @brief: returns the WARP (Windows Advanced Rasterization Processor) adapter
	extern Ptr<IDXGIAdapter4> GetWARPAdapter();

	// @brief: returns the graphics adapter with the most video memory
	//	it is expected to be the most powerful graphics adapter
	// @return: most favored graphics adapter
	extern Ptr<IDXGIAdapter4> GetFavoredAdapter();

	// @brief: returns the graphics adapter with the most video memory
	//	it is expected to be the most powerful graphics adapter
	// @param adapters: list of adapters to choose from
	// @return: most favored graphics adapter
	extern Ptr<IDXGIAdapter4> GetFavoredAdapter(
		const std::vector<Ptr<IDXGIAdapter4>> adapters
	);

	// @brief: checks whether the graphics output supports
	//	tearing (or vsync-off or G-Sync or FreeSync)
	extern bool CheckTearingSupport();

	extern Ptr<ID3D12DescriptorHeap> CreateDescriptorHeap(
		Ptr<ID3D12Device> device,
		D3D12_DESCRIPTOR_HEAP_TYPE type,
		uint32_t numDescriptors,
		D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE
	);

}
