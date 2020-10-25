#pragma once
#include "Convert.hpp"
#include "Filesystem.hpp"
#include "Literals.hpp"
#include "Flags.hpp"
#include "Logger.hpp"

#include "Eigen/Dense"

namespace orbit
{

	using namespace Eigen;

	template<size_t _Buffer_size = 1024, typename... _Format>
	std::string FormatString(std::string_view msg, _Format... args)
	{
		// create a stack-local char buffer
		char buffer[_Buffer_size];

		auto len = sprintf_s(buffer, msg.data(), args...);
		if (len < 0)
		{
			// we won't format strings that will become larger than 4 * 1024 bytes
			if constexpr (_Buffer_size * 2 >= 4_KiB)
				throw std::exception("Message larger than 4KiB. ");
			else return FormatString<_Buffer_size * 2>(msg, args...);
		}

		return std::string(buffer, len + 1);
	}

	template<size_t _Buffer_size = 1024, typename... _Format>
	std::wstring FormatString(std::wstring_view msg, _Format... args)
	{
		// create a stack-local wchar_t buffer
		wchar_t buffer[_Buffer_size];

		auto len = swprintf_s(buffer, msg.data(), args...);
		if (len < 0)
		{
			// we won't format strings that will become larger than 4 * 1024 bytes
			if constexpr (_Buffer_size * 2 >= 4_KiB)
				throw std::exception("Message larger than 4KiB. ");
			else return FormatString<_Buffer_size * 2>(msg, args...);
		}

		return std::wstring(buffer, len + 1);
	}

}
