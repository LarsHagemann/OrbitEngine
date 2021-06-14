#include "Engine/Bindings/DX12/UploadBuffer.hpp"
#include "Engine/Misc/Math.hpp"
#include "Engine/Engine.hpp"

namespace orbit 
{

	std::shared_ptr<UploadBuffer::Page> UploadBuffer::RequestPage()
	{
		std::shared_ptr<Page> page;
		if (!_availablePages.empty())
		{
			page = _availablePages.front();
			_availablePages.pop_front();
		}
		else
		{
			page = std::make_shared<Page>(_pagesize);
			_pages.emplace_back(page);
		}

		return page;
	}

	UploadBuffer::UploadBuffer(size_t pagesize) :
		_pagesize(pagesize)
	{
		ORBIT_INFO_LEVEL(FormatString("Initializing UploadBuffer with pagesize <%d>", pagesize), 13);
	}

	UploadBuffer::Allocation UploadBuffer::Allocate(size_t sizeInBytes, size_t alignment)
	{
		if (sizeInBytes > _pagesize)
			throw std::bad_alloc();

		if (!_currentPage || !_currentPage->CanAllocate(sizeInBytes, alignment))
		{
			_currentPage = RequestPage();
			if (!_currentPage->CanAllocate(sizeInBytes, alignment))
				throw std::bad_alloc();
		}

		return _currentPage->Allocate(sizeInBytes, alignment);
	}

	void UploadBuffer::Reset()
	{
		_currentPage = nullptr;
		_availablePages = _pages;

		for (auto page : _availablePages)
			page->Reset();
	}

	UploadBuffer::Page::~Page()
	{
		_pageResource->Unmap(0, nullptr);
		_CPUPtr = nullptr;
		_GPUPtr = D3D12_GPU_VIRTUAL_ADDRESS(0);
	}

	UploadBuffer::Page::Page(size_t pagesize) :
		_pagesize(pagesize),
		_CPUPtr(nullptr),
		_GPUPtr(0),
		_offset(0)
	{
        ORBIT_INFO_LEVEL(FormatString("Creating UploadBuffer::Page<%d>.", _pagesize), 8);
		ORBIT_THROW_IF_FAILED(Engine::Get()->GetDevice()->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(_pagesize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(_pageResource.GetAddressOf())),
			"Failed to create comitted resource."
		);
		_pageResource->SetName(FormatString(L"Pageresource of size <%zd>", pagesize).c_str());

		_GPUPtr = _pageResource->GetGPUVirtualAddress();
		_pageResource->Map(0, nullptr, &_CPUPtr);
	}

	bool UploadBuffer::Page::CanAllocate(size_t sizeInBytes, size_t alignment) const
	{
		auto nextAlignedOffset = (_offset + alignment - 1) & ~alignment;
		auto nextAlignedSize = (sizeInBytes + alignment - 1) & ~alignment;

		return nextAlignedOffset + nextAlignedSize <= _pagesize;
	}

	UploadBuffer::Allocation UploadBuffer::Page::Allocate(size_t sizeInBytes, size_t alignment)
	{
		auto nextAlignedSize = Math<size_t>::AlignUp(sizeInBytes, alignment);
		_offset              = Math<size_t>::AlignUp(_offset, alignment);

		Allocation allocation;
		allocation.CPU = static_cast<uint8_t*>(_CPUPtr) + _offset;
		allocation.GPU = _GPUPtr + _offset;

		_offset += nextAlignedSize;
		return allocation;
	}

	void UploadBuffer::Page::Reset()
	{
		_offset = 0;
	}

}