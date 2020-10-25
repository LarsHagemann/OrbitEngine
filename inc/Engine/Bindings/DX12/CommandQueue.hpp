#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "DX12Helper.hpp"

#include <queue>
#include <mutex>

namespace orbit
{

	template<class COM>
	using Ptr = Microsoft::WRL::ComPtr<COM>;

	class CommandQueue
	{
	protected:
		// helper struct for fences and command allocators
		struct CommandAllocator
		{
			uint64_t cmd_fence;
			Ptr<ID3D12CommandAllocator> allocator;
		};

		// @member: the type of command lists to be created
		D3D12_COMMAND_LIST_TYPE _cmdListType;
		// @member: the command queue that processes all the command lists
		Ptr<ID3D12CommandQueue> _commandQueue;
		// @member: fence object for CPU/GPU syncronization
		Ptr<ID3D12Fence> _fence;
		// @member: event handle for CPU/GPU syncronization
		HANDLE _fenceEvent;
		// @member: the next fence value to be used
		uint64_t _cachedFenceValue;

		// helper typedef
		using CommandListQueue = std::queue<Ptr<ID3D12GraphicsCommandList>>;
		// helper typedef
		using CommandAllocatorQueue = std::queue<CommandAllocator>;

		// @member: dx12 device
		Ptr<ID3D12Device> _device;
		// @member: queue of command lists that can potentially be used
		CommandListQueue _cmdListQueue;
		// @member: queue of command allocators that can potentially be used
		CommandAllocatorQueue _cmdAllocatorQueue;

		std::mutex _creationMutex;
	protected:
		// @method: creates a new command allocator
		// @internal
		Ptr<ID3D12CommandAllocator> CreateCommandAllocator();
		// @method: creates a new command list with a specified command allocator
		// @internal
		Ptr<ID3D12GraphicsCommandList> CreateCommandList(Ptr<ID3D12CommandAllocator> allocator);
	public:
		// @brief: constructs a new command queue with a certain command list type
		// @param device: the initialized direct3d device to be used
		// @param type: the command list type to be used
		//	usually, you want D3D12_COMMAND_LIST_TYPE_DIRECT
		CommandQueue(Ptr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type);
		// @brief: destructor
		~CommandQueue();
		// @method: returns a command list that can be used to render
		// @return: the next available command list (might construct a new one)
		Ptr<ID3D12GraphicsCommandList> GetCommandList();
		// @method: executes a command list
		// @param cmdList: the command list to be executed on the local command queue
		// @return: fence for synchronisation
		uint64_t ExecuteCommandList(Ptr<ID3D12GraphicsCommandList> cmdList);
		// @method: executes a list of command lists
		// @param cmdLists: vector of command lists
		// @return: fence for synchronisation
		uint64_t ExecuteCommandLists(std::vector<Ptr<ID3D12GraphicsCommandList>> cmdLists);
		// @method: signals the command queue
		uint64_t Signal();
		// @method: tests whether a certain fence value has been reached
		// @param fenceValue: the fence value to test again
		bool IsFenceComplete(uint64_t fenceValue);
		// @method: waits for a certain fence value to be reached
		// @param fenceValue: the fence value to wait for
		void WaitForFenceValue(uint64_t fenceValue);
		// @method: stalls until the command queue is empty and every 
		//	command has been processed
		void Flush();
		// @method: returns the internally used command queue
		Ptr<ID3D12CommandQueue> GetCommandQueue() const { return _commandQueue; }
	};

}
