#pragma once
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/Platform.hpp"
#include "Engine/Misc/Literals.hpp"

#include <map>
#include <deque>
#include <memory>

namespace orbit
{

    class UploadBuffer
	{
	public:
		// @brief: wrapper for an allocation
		struct Allocation
		{
			// @member: the aligned CPU base pointer
			//	copy your data (via memcpy) to this address
			void* CPU;
			// @member: GPU address of the uploaded data
			D3D12_GPU_VIRTUAL_ADDRESS GPU;
		};
	protected:
		// @brief: allocator page of memory
		class Page
		{
		protected:
			// @member: the internal page resource
			Ptr<ID3D12Resource> _pageResource;
			// @member: memory base pointer
			void* _CPUPtr;
			// @member: GPU base pointer
			D3D12_GPU_VIRTUAL_ADDRESS _GPUPtr;
			// @member: this page's size
			const size_t _pagesize;
			// @member: the current offset (how much memory
			//	of the page is in use)
			size_t _offset;
		public:
			~Page();
			// @brief: creates a new memory page with a given pagesize
			// @param pagesize: the page's size
			Page(size_t pagesize);
			// @method: tests whether a certain allocation can be made
			//	on this page
			// @param sizeInBytes: requested memory size
			// @param alignment: requested memory alignment
			// @return: true if the allocation can be made on this page
			bool CanAllocate(size_t sizeInBytes, size_t alignment) const;
			// @method: does a certain allocation on this page
			// @param sizeInBytes: requested memory size
			// @param alignment: requested memory alignment
			// @return: see struct UploadBuffer::Allocation
			// @throws: std::bad_alloc if sizeInBytes exceeds the page size
			//	or the allocation failed for some other reason
			Allocation Allocate(size_t sizeInBytes, size_t alignment);
			// @method: resets the page for reuse
			void Reset();
		};
		using PagePool = std::deque<std::shared_ptr<Page>>;
		// @member: this UploadBuffer's pagesize
		const size_t _pagesize;
		// @member: pool of all pages
		PagePool _pages;
		// @member: pool of all usable pages
		PagePool _availablePages;
		// @member: the page currently in use
		std::shared_ptr<Page> _currentPage;
	protected:
		// @method: returns a page from the pool of pages
		//	or creates a new page
		std::shared_ptr<Page> RequestPage();
	public:
		// @brief: creates a new UploadBuffer with a given page size
		explicit UploadBuffer(size_t pageSize = 2_MiB);
		// @method: allocates memory in an upload heap
		// @param sizeInBytes: the requested allocation size in bytes
		// @param alignment: the alignment of the memory in the page
		// @return: see struct UploadBuffer::Allocation
		//	for example for a transformation matrix in a constant buffer
		//	you would call UploadBuffer::Allocate(sizeof(XMMATRIX)/4, 256);
		Allocation Allocate(size_t sizeInBytes, size_t alignment);
		// @method: recycles the memory. Note that this function does not 
		//	return any memory to the operating system. It just makes it usable for
		//	the next frame. Also, the execution of this buffer's command list
		//	on the command queue must have finished prior to calling Reset();
		void Reset();
	};

}

