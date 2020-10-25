#include "Engine/Management/AllocatorPage.hpp"
#include "Engine/Misc/Logger.hpp"
#include "Engine/Engine.hpp"

namespace orbit
{

	void Allocation::Free() const
	{
		_page->Free(*this);
	}

	AllocatorPage::OffsetType AllocatorPage::ComputeOffset(void* handle)
	{
        return (uint8_t*)handle - (uint8_t*)_baseHandle;
	}

	void AllocatorPage::AddNewBlock(OffsetType offset, size_t bytes)
	{
		auto offsetIt = _freeListByOffset.emplace(offset, bytes);
		auto sizeIt = _freeListBySize.emplace(bytes, offsetIt.first);
		offsetIt.first->second.freeListBySizeIt = sizeIt;
	}

	void AllocatorPage::FreeBlock(OffsetType offset, size_t bytes)
	{
		auto nextBlockIt = _freeListByOffset.upper_bound(offset);

		// Find the block that appears before the block being freed.
		auto prevBlockIt = nextBlockIt;
		// If it's not the first block in the list.
		if (prevBlockIt != _freeListByOffset.begin())
		{
			// Go to the previous block in the list.
			--prevBlockIt;
		}
		else
		{
			// Otherwise, just set it to the end of the list to indicate that no
			// block comes before the one being freed.
			prevBlockIt = _freeListByOffset.end();
		}

		// Add the number of free handles back to the heap.
		// This needs to be done before merging any blocks since merging
		// blocks modifies the numDescriptors variable.
		_numFreeBytes += bytes;

		if (prevBlockIt != _freeListByOffset.end() &&
			offset == prevBlockIt->first + prevBlockIt->second.size)
		{
			// The previous block is exactly behind the block that is to be freed.
			//
			// PrevBlock.Offset           Offset
			// |                          |
			// |<-----PrevBlock.Size----->|<------Size-------->|
			//

			// Increase the block size by the size of merging with the previous block.
			offset = prevBlockIt->first;
			bytes += prevBlockIt->second.size;

			// Remove the previous block from the free list.
			_freeListBySize.erase(prevBlockIt->second.freeListBySizeIt);
			_freeListByOffset.erase(prevBlockIt);
		}

		if (nextBlockIt != _freeListByOffset.end() &&
			offset + bytes == nextBlockIt->first)
		{
			// The next block is exactly in front of the block that is to be freed.
			//
			// Offset               NextBlock.Offset 
			// |                    |
			// |<------Size-------->|<-----NextBlock.Size----->|

			// Increase the block size by the size of merging with the next block.
			bytes += nextBlockIt->second.size;

			// Remove the next block from the free list.
			_freeListBySize.erase(nextBlockIt->second.freeListBySizeIt);
			_freeListByOffset.erase(nextBlockIt);
		}

		AddNewBlock(offset, bytes);
	}

	AllocatorPage::AllocatorPage(size_t pagesize)
		//_numDescriptorsInHeap(pagesize)
	{
        _numFreeBytes = pagesize;
        _baseHandle = malloc(pagesize);
		// Initialize the free lists
		AddNewBlock(0, pagesize);
	}

	bool AllocatorPage::CanAllocate(size_t sizeInBytes) const
	{
		return _freeListBySize.lower_bound(sizeInBytes) != _freeListBySize.end();
	}

	size_t AllocatorPage::FreeBytes() const
	{
		return _numFreeBytes;
	}

	Allocation AllocatorPage::Allocate(size_t sizeInBytes)
	{
        ReleaseStaleDescriptors();
		std::lock_guard<std::mutex> lock(_allocationMutex);

		if (sizeInBytes > _numFreeBytes)
		{
			return Allocation();
		}

		// Get the first block that is large enough to satisfy the request.
		auto smallestBlockIt = _freeListBySize.lower_bound(sizeInBytes);
		if (smallestBlockIt == _freeListBySize.end())
		{
			// There was no free block that could satisfy the request.
			return Allocation();
		}

		auto blockSize = smallestBlockIt->first;

		// The pointer to the same entry in the FreeListByOffset map.
		auto offsetIt = smallestBlockIt->second;

		// The offset in the descriptor heap.
		auto offset = offsetIt->first;

		_freeListBySize.erase(smallestBlockIt);
		_freeListByOffset.erase(offsetIt);

		// Compute the new free block that results from splitting this block.
		auto newOffset = offset + sizeInBytes;
		auto newSize = blockSize - sizeInBytes;

		if (newSize > 0)
		{
			// If the allocation didn't exactly match the requested size,
			// return the left-over to the free list.
			AddNewBlock(newOffset, newSize);
		}

		_numFreeBytes -= sizeInBytes;

        return Allocation{ ((uint8_t*)_baseHandle + offset), sizeInBytes, shared_from_this() };
	}

	void AllocatorPage::Free(const Allocation& handle)
	{
		auto offset = ComputeOffset(handle.memory);

		std::lock_guard<std::mutex> lock(_allocationMutex);

		// Don't add the block directly to the free list until the frame has completed.
        _staleBlocks.emplace(offset, handle.size);
	}

	void AllocatorPage::ReleaseStaleDescriptors()
	{
		std::lock_guard<std::mutex> lock(_allocationMutex);

		while (!_staleBlocks.empty())
		{
			auto& block = _staleBlocks.front();

			// The offset of the descriptor in the heap.
			auto offset = block.offset;
			// The number of descriptors that were allocated.
			auto size = block.size;
			_staleBlocks.pop();

			FreeBlock(offset, size);
		}
	}

}
