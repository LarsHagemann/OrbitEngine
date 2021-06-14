#include "Engine/Management/Allocator.hpp"
#include "Engine/Management/AllocatorPage.hpp"
#include "Engine/Misc/Logger.hpp"

#include <algorithm>

namespace orbit
{

	std::shared_ptr<AllocatorPage> Allocator::CreateAllocatorPage()
	{
		ORBIT_INFO_LEVEL(FormatString("Creating new allocator page [%d]", _pagesize), 10);
		auto page = std::make_shared<AllocatorPage>(_pagesize);

		_heapPool.emplace_back(page);
		_availablePages.insert(_heapPool.size() - 1);

		return page;
	}

	Allocator::Allocator(size_t pagesize) :
		_pagesize(pagesize)
	{
		ORBIT_INFO_LEVEL(FormatString("Initializing Allocator with pagesize <%d>", pagesize), 13);
	}

	Allocator::~Allocator()
	{
	}

	Allocation Allocator::CPUAllocate(size_t sizeInBytes)
	{
		std::lock_guard<std::mutex> lock(_allocationMutex);
		Allocation allocation;
		for (auto iter = _availablePages.begin(); iter != _availablePages.end(); ++iter)
		{
			auto allocatorPage = _heapPool[*iter];

			allocation = allocatorPage->Allocate(sizeInBytes);

			if (allocatorPage->FreeBytes() == 0)
			{
				iter = _availablePages.erase(iter);
				if (iter == _availablePages.end())
					break;
			}

			// A valid allocation has been found.
			if (allocation.IsValid())
				return allocation;
		}

		if (!allocation.IsValid())
		{
			_pagesize = std::max(_pagesize, sizeInBytes);
			auto page = CreateAllocatorPage();

			allocation = page->Allocate(sizeInBytes);
		}

		return allocation;
	}

	void Allocator::ReleaseStaleDescriptors()
	{
		std::lock_guard<std::mutex> lock(_allocationMutex);

		for (size_t i = 0; i < _heapPool.size(); ++i)
		{
			auto page = _heapPool[i];

			page->ReleaseStaleDescriptors();

			if (page->FreeBytes() > 0)
			{
				_availablePages.insert(i);
			}
		}
	}

}