#pragma once
#include <memory>
#include <map>
#include <queue>
#include <mutex>

#include "Engine/Misc/Literals.hpp"

namespace orbit
{

    class AllocatorPage;

    struct Allocation
    {
        void* memory = nullptr;
        size_t size = 0u;
        std::shared_ptr<AllocatorPage> _page;
        bool IsValid() const
        {
            return memory != nullptr;
        }
        void Free() const;
    };

    class AllocatorPage : public std::enable_shared_from_this<AllocatorPage>
	{
    private:
        // @brief: The offset (in descriptors) within the descriptor heap.
        using OffsetType = size_t;
        // @brief: The number of descriptors that are available.
        using SizeType = size_t;
        // @see: struct DescriptorAllocatorPage::FreeBlockInfo
        struct FreeBlockInfo;
        // @brief: A map that lists the free blocks by the offset within the descriptor heap.
        using FreeListByOffset = std::map<OffsetType, FreeBlockInfo>;

        // @brief: A map that lists the free blocks by size.
        //  Needs to be a multimap since multiple blocks can have the same size.
        using FreeListBySize = std::multimap<SizeType, FreeListByOffset::iterator>;

        // @brief: abstracts a single free block in the descriptor heap
        struct FreeBlockInfo
        {
            FreeBlockInfo(SizeType size)
                : size(size)
            {}

            // @member: size of the block
            SizeType size;
            // @member: iterator into the size-indexed map
            FreeListBySize::iterator freeListBySizeIt;
        };
        // @brief: abstracts a stale descriptor
        struct StaleBlockInfo
        {
            StaleBlockInfo(OffsetType offset, SizeType size)
                : offset(offset)
                , size(size)
            {}

            // @brief: The offset within the descriptor heap.
            OffsetType offset;
            // @brief: The number of descriptors
            SizeType size;
        };
        // @brief: Stale descriptors are queued for release until the frame that they were freed
        // has completed.
        using StaleBlocksQueue = std::queue<StaleBlockInfo>;
        // @member: list of free blocks indexed by their offset
        FreeListByOffset _freeListByOffset;
        // @member: list of free blocks indexed by their size
        FreeListBySize _freeListBySize;
        // @member: queue of stale descriptors
        StaleBlocksQueue _staleBlocks;
        // @member: allocation mutex
        std::mutex _allocationMutex;
        // @member: page memory
        void* _baseHandle;
        // @member: page size
        size_t _pagesize;
        // @member: number of bytes currently free
        size_t _numFreeBytes;
    protected:
        // @method: Compute the offset of the handle from the start of the page.
        // @param handle: pointer to the memory to compute the offset
        OffsetType ComputeOffset(void* handle);

        // @method: Adds a new block to the free list.
        void AddNewBlock(OffsetType offset, size_t bytes);

        // @method: Free a block of bytes.
        //  This will also merge free blocks in the free list to form larger blocks
        //  that can be reused.
        void FreeBlock(OffsetType offset, size_t bytes);
	public:
        // @brief: create a new descriptor allocator page
        // @param device: dx12 device
        // @param type: the heap type to create
        // @param numDescriptors: the number of descriptors in this heap
        AllocatorPage(size_t pagesize = 2_MiB);

        // @method: tests whether this page can allocate
        //  a certain number of bytes
        // @param sizeInBytes: the number of bytes to test for
        // @return: true if allocation will be successful
        bool CanAllocate(size_t sizeInBytes) const;

        // @method: allocates a certain number of bytes
        // @param sizeInBytes: number of bytes to allocate
        // @return: see struct Allocation
        Allocation Allocate(size_t sizeInBytes);

        // @method: frees a allocation on a certain framenumber
        // @param descriptorHandle: descriptor allocation to free
        // @param frameNumber: current frame number
        // @note: stale descriptors are not freed immediately but
        //  when ReleaseStaleDescriptors() is being called
        void Free(const Allocation& descriptorHandle);

        // @brief: frees descriptors to the heap for a certain frameNumber
        void ReleaseStaleDescriptors();

        size_t FreeBytes() const;
	};

}
