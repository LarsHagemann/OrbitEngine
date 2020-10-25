#pragma once
#include "Filesystem.hpp"

namespace orbit
{

	static constexpr size_t operator ""_KiB(size_t in) { return in * 1024; }
	static constexpr size_t operator ""_KB(size_t in) { return in * 1000; }
	static constexpr size_t operator ""_MiB(size_t in) { return in * 1024 * 1024; }
	static constexpr size_t operator ""_MB(size_t in) { return in * 1000 * 1000; }

	static fs::path operator ""_path(const char* ptr, size_t len)
	{
		return fs::path(std::string(ptr, len), fs::path::generic_format);
	}

	static fs::path operator ""_path(const wchar_t* ptr, size_t len)
	{
		return fs::path(std::wstring(ptr, len), fs::path::generic_format);
	}

	static fs::path operator ""_path(const char16_t* ptr, size_t len)
	{
		return fs::path(std::basic_string<char16_t>(ptr, len), fs::path::generic_format);
	}

	static fs::path operator ""_path(const char32_t* ptr, size_t len)
	{
		return fs::path(std::basic_string<char32_t>(ptr, len), fs::path::generic_format);
	}

}
