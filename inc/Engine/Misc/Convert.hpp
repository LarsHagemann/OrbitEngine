#pragma once
#include "Engine/Bindings/Platform.hpp"

#include <string_view>
#include <string>

namespace orbit
{

	static void AsciiToWideString(std::string_view aStr, std::wstring* wStr)
	{
#ifdef ORBIT_WIN
		auto len = MultiByteToWideChar(
			CP_UTF8, 
			0, 
			aStr.data(), 
			static_cast<int>(aStr.size()), 
			nullptr, 
			0
		);
		wStr->resize(len);
		MultiByteToWideChar(
			CP_UTF8, 
			0, 
			aStr.data(), 
			static_cast<int>(aStr.size()), 
			wStr->data(), 
			len
		);
#endif
	}

	static void WideToAsciiString(std::wstring_view wStr, std::string* aStr)
	{
#ifdef ORBIT_WIN
		BOOL useDefault = 0;
		CCHAR defaultChar = 0;
		auto len = WideCharToMultiByte(
			CP_UTF8, 
			0, 
			wStr.data(), 
			static_cast<int>(wStr.size()), 
			nullptr, 
			0, 
			&defaultChar, 
			&useDefault
		);
		aStr->resize(len);
		WideCharToMultiByte(
			CP_UTF8, 
			0,
			wStr.data(), 
			static_cast<int>(wStr.size()), 
			aStr->data(), 
			len, 
			&defaultChar, 
			&useDefault
		);
#endif
	}

	static std::wstring AsciiToWideString(std::string_view aStr)
	{
		std::wstring tmp;
		AsciiToWideString(aStr, &tmp);
		return tmp;
	}

	static std::string WideToAsciiString(std::wstring_view wStr)
	{
		std::string tmp;
		WideToAsciiString(wStr, &tmp);
		return tmp;
	}

}
