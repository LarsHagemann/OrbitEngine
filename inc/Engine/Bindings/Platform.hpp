#pragma once
#if defined _WIN32 || defined _WIN64
#ifndef ORBIT_WIN
#define ORBIT_WIN
#include <Windows.h>
#include <ShlObj.h>
#endif
#else
#ifndef ORBIT_UNIX
#define ORBIT_UNIX

#endif
#endif

#ifdef ORBIT_DX12
#include <d3d12.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#elif defined ORBIT_DX11
#include <d3d11.h>
#include <D3DX11.h>
#include <d3dcompiler.h>
namespace orbit
{
	using BufferHeap = CPUAllocator;
}
#elif defined ORBIT_OPENGL

#endif

#include "Engine/Misc/Filesystem.hpp"

#include <thread>
#include <string_view>

#include <chrono>

namespace orbit
{
#ifdef ORBIT_WIN
	using window_handle = HWND;
#else
	using window_handle = int*;
#endif

	namespace pt {
		static void Sleep(size_t millis)
		{
#ifdef ORBIT_WIN
			::Sleep(static_cast<DWORD>(millis));
#endif
		}

		static void SetThreadName(std::thread& thread, std::wstring_view name)
		{
#ifdef ORBIT_WIN
			::SetThreadDescription(
				thread.native_handle(),
				name.data()
			);
#endif
		}

		static uint64_t GetFileModifiedDate(const fs::path& filepath)
		{
#ifdef ORBIT_WIN
			FILETIME ft;
			auto handle = ::CreateFileW(
				filepath.c_str(), 
				GENERIC_READ, 
				0, 
				nullptr, 
				OPEN_EXISTING, 
				FILE_ATTRIBUTE_NORMAL, 
				nullptr
			);
			if (handle == INVALID_HANDLE_VALUE)
				return 0ULL;
			::GetFileTime(handle, nullptr, nullptr, &ft);
			::CloseHandle(handle);
			auto nano100 = (LONGLONG)ft.dwLowDateTime + ((LONGLONG)(ft.dwHighDateTime) << 32LL);
			return (nano100 / 10000ULL) - 116444736000000000ULL;
#else
			struct stat result;
			if(stat(filepath.generic_string().c_str(), &result) == 0)
				return static_cast<uint64_t>(result.st_mtime);
			
			return 0;
#endif
		}

		static fs::path GetAppdataPath()
		{
#ifdef ORBIT_WIN
			PWSTR buffer;
			::SHGetKnownFolderPath(
				FOLDERID_RoamingAppData,
				0,
				0,
				&buffer
			);
			return fs::path(buffer);
#else
			return "/usr/.orbit/";
#endif
		}

	}

}
