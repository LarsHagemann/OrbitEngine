#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/DX/DXInput.hpp"
#include "DX12ResourceManager.hpp"
#include "CommandQueue.hpp"
#include "Engine/Misc/Helper.hpp"

#include <vector>
#include <memory>
#include <mutex>

#include <wrl/client.h>

namespace orbit
{

	class Renderer;

	struct InitDesc
	{
		Ptr<IDXGIAdapter> adapter;
		bool useWARP;
		unsigned sampleCount;
		unsigned sampleQuality;
		unsigned numBackbuffers;
		unsigned numThreads;
		static InitDesc GetDefaultDesc();
	};

	class EngineResources : public DXInputResource, public DX12ResourceManager
	{
	protected:
		std::shared_ptr<CommandQueue> _directCommandQueue;
		std::shared_ptr<CommandQueue> _copyCommandQueue;
		std::shared_ptr<CommandQueue> _computeCommandQueue;
		// @member: the DirectX 12 rendering device
		Ptr<ID3D12Device> _device;
		// @member: descriptor heap for Render Target Views
		Ptr<ID3D12DescriptorHeap> _RTVDescriptorHeap;
		// @member: descriptor heap for CBV/SRV/UAVs
		Ptr<ID3D12DescriptorHeap> _CBVDescriptorHeap;
		// @member: descriptor heap for DSVs
		Ptr<ID3D12DescriptorHeap> _DSVDescriptorHeap;
		// @member: the DXGI swap chain
		Ptr<IDXGISwapChain3> _swapChain;
		// @member: the syncronization fence
		Ptr<ID3D12Fence> _fence;
		// @member: backbuffers for the swap chain
		std::vector<Ptr<ID3D12Resource>> _backbuffers;
		// @member: fences for the back buffer resources
		std::vector<uint64_t> _fences;
		// @member: command lists used for rendering the current frame
		std::vector<Ptr<ID3D12GraphicsCommandList>> _commandLists;
		//Ptr<ID3D12GraphicsCommandList> _commandList;
		// @member: handle to the RTV heap
		D3D12_CPU_DESCRIPTOR_HANDLE _rtvHandle;
		// @member: Depth stencil resource
		Ptr<ID3D12Resource> _dsvBuffer;
		// @member: handle to the DSV heap
		D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle;
		// @member: rendering viewport
		D3D12_VIEWPORT _viewport;
		// @member: scissor rect
		D3D12_RECT _scissorRect;
		// @member: the number of backbuffers
		const unsigned _numBackbuffers;
		// @member: the associated window handle
		HWND _hWnd;
		// @member: the current backbuffer
		unsigned _currentBackbuffer;
		// @member: is tearing supported by the graphics card and the output?
		bool _tearingSupported;
		// @member: should V-Sync be used?
		bool _vSync;
		// @member: index for sequencing renderers
		unsigned _rendererIndex;
		// @member: the number of threads handling the object updating
		unsigned _numThreads;
		// @member: synchronization mutex
		std::mutex _listMutex;
	protected:
		void ResizeDepthBuffer();
		void UpdateRenderTargetViews();
		void OnResize();
		void Clear(Vector4f color = Vector4f::Ones());
		void Display();
		void Cleanup();
		void InitResources();
		unsigned GetParallelRenderCount() const { return _numThreads; }
		[[nodiscard]] Renderer CreateRenderer();
		Ptr<ID3D12GraphicsCommandList> EmplaceCommandList();
		[[nodiscard]] Renderer GetDefaultRenderer() const;
	public:
		EngineResources(HWND hWnd, InitDesc* desc);
		virtual ~EngineResources();
		Ptr<ID3D12Device> GetDevice() const { return _device; }
		std::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE type);
		static fs::path GetEngineFolder();
	};

}
