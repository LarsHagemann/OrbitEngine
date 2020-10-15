#include "exception.hpp"
#include <iostream>
#include <sstream>

namespace orbit
{

	void OrbitLogger::print(std::ostream& stream, std::string_view prefix, std::string_view message, const char* function, unsigned line)
	{
		std::time_t t = std::time(0);   // get time now
		std::tm now;
		localtime_s(&now, &t);

		std::ostringstream ss;
		ss << '[' << (now.tm_year + 1900) << '-'
			<< (now.tm_mon + 1) << '-'
			<< now.tm_mday 
			<< "  " << now.tm_hour 
			<< ':' << now.tm_min
			<< ':' << now.tm_sec
			<< "] ";

		stream
			<< ss.str()
			<< prefix
			<< " "
			<< message
			<< " - in <"
			<< function
			<< ':'
			<< line
			<< ">\n";
	}

	void OrbitLogger::print(Severity severity, std::string_view message, const char* function, unsigned line)
	{
		std::string prefix;
		switch (severity)
		{
		case Severity::S_INFO: prefix = "[INFO]"; break;
		case Severity::S_WARN: prefix = "[WARNING]"; break;
		case Severity::S_ERR:  prefix = "[ERROR]"; break;
		}

		switch (severity)
		{
		case orbit::OrbitLogger::Severity::S_INFO:
			print(sInfoStream, prefix, message, function, line);
			break;
		case orbit::OrbitLogger::Severity::S_WARN:
			print(sWarnStream, prefix, message, function, line);
			break;
		case orbit::OrbitLogger::Severity::S_ERR:
			print(sErrorStream, prefix, message, function, line);
			break;
		default:
			// invalid severity level
			print(
				Severity::S_WARN, 
				"You tried calling OrbitLogger::print(...) with an invalid severity level.",
				__FUNCTION__,
				__LINE__
			);
			print(
				Severity::S_WARN,
				message,
				function,
				line
			);
			break;
		}
	}

	void OrbitLogger::print(Severity severity, std::string_view message, HRESULT result, const char* function, unsigned line)
	{
		auto msg = std::string(message);
		auto resMsg = std::system_category().message(result);
		msg += " {" + std::to_string(result) + "}{" + resMsg + "}";
		print(severity, msg, function, line);
	}

	void OrbitLogger::printLevel(std::string_view message, unsigned level, const char* function, unsigned line)
	{
		if (level <= sInfoLevel)
			print(Severity::S_INFO, message, function, line);
	}

	std::ostream& OrbitLogger::sInfoStream = std::cout;
	std::ostream& OrbitLogger::sWarnStream = std::clog;
	std::ostream& OrbitLogger::sErrorStream = std::cerr;
	unsigned OrbitLogger::sInfoLevel = 0xFFFFFFFFu;

}