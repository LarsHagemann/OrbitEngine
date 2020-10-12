#include "command_queue.hpp"
#include "exception.hpp"

namespace orbit
{

	Ptr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
	{
		Ptr<ID3D12CommandAllocator> allocator;
		ORBIT_THROW_IF_FAILED(_device->CreateCommandAllocator(
				_cmdListType,
				IID_PPV_ARGS(allocator.GetAddressOf())
			),
			"Failed to create command allocator."
		);
		return allocator;
	}

	Ptr<ID3D12GraphicsCommandList> CommandQueue::CreateCommandList(Ptr<ID3D12CommandAllocator> allocator)
	{
		Ptr<ID3D12GraphicsCommandList> cmdList;
		ORBIT_THROW_IF_FAILED(_device->CreateCommandList(
				0,
				_cmdListType,
				allocator.Get(),
				nullptr,
				IID_PPV_ARGS(cmdList.GetAddressOf())
			),
			"Failed to create command list."
		);
		return cmdList;
	}

	CommandQueue::CommandQueue(Ptr<ID3D12Device> device, D3D12_COMMAND_LIST_TYPE type) :
		_device(device),
		_cmdListType(type),
		_cachedFenceValue(0)
	{
		D3D12_COMMAND_QUEUE_DESC desc;
		ZeroMemory(&desc, sizeof(D3D12_COMMAND_QUEUE_DESC));
		desc.Type = type;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;

		ORBIT_THROW_IF_FAILED(
			_device->CreateCommandQueue(&desc, IID_PPV_ARGS(_commandQueue.GetAddressOf())),
			"Failed to create command queue."
		);
		ORBIT_THROW_IF_FAILED(
			_device->CreateFence(
				_cachedFenceValue,
				D3D12_FENCE_FLAG_NONE,
				IID_PPV_ARGS(_fence.GetAddressOf())
			),
			"Failed to create fence object."
		);

		_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(_fenceEvent && "Failed to create fence event handle.");
	}

	CommandQueue::~CommandQueue()
	{

	}

	Ptr<ID3D12GraphicsCommandList> CommandQueue::GetCommandList()
	{
		Ptr<ID3D12GraphicsCommandList> commandList;
		Ptr<ID3D12CommandAllocator> commandAllocator;

		if (!_cmdAllocatorQueue.empty() &&
			IsFenceComplete(_cmdAllocatorQueue.front().cmd_fence))
		{
			commandAllocator = _cmdAllocatorQueue.front().allocator;
			_cmdAllocatorQueue.pop();

			ORBIT_THROW_IF_FAILED(
				commandAllocator->Reset(),
				"Failed to reset command allocator."
			);
		}
		else
			commandAllocator = CreateCommandAllocator();

		if (!_cmdListQueue.empty())
		{
			commandList = _cmdListQueue.front();
			_cmdListQueue.pop();

			ORBIT_THROW_IF_FAILED(
				commandList->Reset(commandAllocator.Get(), nullptr),
				"Failed to reset command list."
			);
		}
		else
			commandList = CreateCommandList(commandAllocator);

		commandList->SetPrivateDataInterface(
			__uuidof(ID3D12CommandAllocator),
			commandAllocator.Get()
		);

		return commandList;
	}

	uint64_t CommandQueue::ExecuteCommandList(Ptr<ID3D12GraphicsCommandList> cmdList)
	{
		cmdList->Close();

		ID3D12CommandAllocator* allocator;
		UINT allocatorSize = sizeof(ID3D12CommandAllocator);
		ORBIT_THROW_IF_FAILED(
			cmdList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &allocatorSize, &allocator),
			"Failed to retrieve command allocator from command list."
		);

		ID3D12CommandList* const ppCommandLists[] = {
			cmdList.Get()
		};

		_commandQueue->ExecuteCommandLists(1, ppCommandLists);
		uint64_t fenceValue = Signal();

		_cmdAllocatorQueue.emplace(CommandAllocator{ fenceValue, allocator });
		_cmdListQueue.push(cmdList);

		allocator->Release();

		return fenceValue;
	}

	uint64_t CommandQueue::Signal()
	{
		ORBIT_THROW_IF_FAILED(_commandQueue->Signal(
			_fence.Get(), 
			_cachedFenceValue),
			"Failed to signal command queue."
		);
		return _cachedFenceValue++;
	}

	bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
	{
		return _fence->GetCompletedValue() >= fenceValue;
	}

	void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
	{
		if (!IsFenceComplete(fenceValue))
		{
			ORBIT_THROW_IF_FAILED(
				_fence->SetEventOnCompletion(fenceValue, _fenceEvent),
				"Failed to set event on completion."
			);
			::WaitForSingleObject(_fenceEvent, INFINITE);
		}
	}

	void CommandQueue::Flush()
	{
		WaitForFenceValue(Signal());
	}

}