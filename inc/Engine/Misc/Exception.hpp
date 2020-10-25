#pragma once
#include <exception>
#include <string_view>

#include "Helper.hpp"

namespace orbit
{

	class Exception : public std::exception
	{
	public:
		Exception(std::string_view msg) :
			std::exception(msg.data())
		{}
		template<typename... _Format>
		Exception(std::string_view msg, _Format... args) :
			std::exception(FormatString(msg, args...).data())
		{}
	};

}
