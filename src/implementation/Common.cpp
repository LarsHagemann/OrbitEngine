#include "implementation/Common.hpp"

#if defined _DEBUG

const char* monitor_file = __FILE__;
unsigned    monitor_line = 0;

bool prepare_monitoring()
{
	alloc_dict.allocs = (AllocEntry*)malloc(sizeof(AllocEntry));
	alloc_dict.size = 0;
	alloc_dict.capacity = 1;
    return true;
}

void grow_dictionary()
{
	auto tmp = alloc_dict.allocs;
	alloc_dict.allocs = (AllocEntry*)realloc(alloc_dict.allocs, (alloc_dict.capacity *= 2) * sizeof(AllocEntry));
    if (alloc_dict.allocs == nullptr)
    {
        free(tmp);
        throw("Failed to reallocate in memory monitoring.");
    }
}

void shutdown_monitoring()
{
	auto sum = 0u;
	for (auto i = 0u; i < alloc_dict.size; ++i)
	{
		const auto& entry = alloc_dict.allocs[i];
		printf_s("Memory leaked at '%s':%d: %d bytes\n", entry.file, entry.line, entry.size);
        free((void*)entry.file);
		sum += entry.size;
	}
	printf_s("Memory leaked in total  : %d bytes\n", sum);

	free(alloc_dict.allocs);
    alloc_dict.allocs = nullptr;
}

void log_allocations()
{
	auto sum = 0u;
	for (auto i = 0u; i < alloc_dict.size; ++i)
	{
		const auto& entry = alloc_dict.allocs[i];
		printf_s("Open allocation at '%s':%d: %d bytes\n", entry.file, entry.line, entry.size);
		sum += entry.size;
	}
	printf_s("Open allocations in total : %d bytes\n", sum);
}

void monitor_allocation(const void* alloc, unsigned size, const char* file, unsigned line)
{
	if (alloc_dict.capacity == alloc_dict.size)
		grow_dictionary();

	alloc_dict.allocs[alloc_dict.size++] = AllocEntry{ alloc, file, line, size };
}

void monitor_deallocation(const void* alloc)
{
	if (!alloc_dict.allocs)
		return;

	for (auto i = 0u; i < alloc_dict.size; ++i)
	{
		if (alloc_dict.allocs[i].alloc == alloc)
		{
			std::memmove(
				alloc_dict.allocs + i,
				alloc_dict.allocs + i + 1, 
				(alloc_dict.size - (i + 1)) * sizeof(AllocEntry)
			);
			alloc_dict.size--;
			return;
		}
	}
}

bool prepare_alloc(const char* file, unsigned line)
{
	monitor_file = file;
	monitor_line = line;
	return false;
}

#endif
