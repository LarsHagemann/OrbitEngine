#pragma once
#include <vector>
#include <memory>
#include <set>
#include <mutex>

#include "AllocatorPage.hpp"
#include "Engine/Misc/Literals.hpp"

namespace orbit
{

	class Allocator
	{
	protected:
		using MemoryHeapPool = std::vector<std::shared_ptr<AllocatorPage>>;
		// @member: the number of descriptors per heap
		size_t _pagesize;
		// @member: pool of descriptor heaps
		MemoryHeapPool _heapPool;
		// @member: indices of available pages in the DescriptorHeapPool
		std::set<size_t> _availablePages;
		// @member: mutex for thread safe allocations
		std::mutex _allocationMutex;
	protected:
		// @method: creates a new descriptor heap with a certain
		//	number of descriptors
		std::shared_ptr<AllocatorPage> CreateAllocatorPage();
	public:
		// @brief: creates a new descriptor allocator of a certain descriptor heap type
		// @param pagesize: the number of bytes in a page
		Allocator(size_t pagesize = 2_MiB);
		// @destructor
		virtual ~Allocator();

		// @brief: allocates a number of contiguous bytes
		// @param sizeInBytes: the number of bytes to allocate
		Allocation CPUAllocate(size_t sizeInBytes);

		// @brief: releases all the stale descriptors
		// @param frameNumber: 
		void ReleaseStaleDescriptors();
	};

}
