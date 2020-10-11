#pragma once
#include <string>
#include <ostream>
#include "helper.hpp"

namespace orbit
{

	// @brief: helper class for logging information to
	//	info, warning and error streams
	class OrbitLogger
	{
	public:
		enum class Severity
		{
			S_INFO,
			S_WARN,
			S_ERR
		};
	protected:
		// @brief: internal helper function for logging
		static void print(
			std::ostream& stream,
			std::string_view prefix,
			std::string_view message,
			const char* function,
			unsigned line
		);
	public:
		// @member: this is where messages with S_INFO are written to
		//	at initialization time you can take an arbitrary ostream (std::ostringstream, std::ofstream, ...)
		//	and override this member
		//		orbit::OrbitLogger::sInfoStream.rdbuf(my_ostream.rdbuf());
		static std::ostream& sInfoStream;
		// @member: this is where messages with S_WARN are written to
		//	at initialization time you can take an arbitrary ostream (std::ostringstream, std::ofstream, ...)
		//	and override this member
		//		orbit::OrbitLogger::sWarnStream.rdbuf(my_ostream.rdbuf());
		static std::ostream& sWarnStream;
		// @member: this is where messages with S_ERR are written to
		//	at initialization time you can take an arbitrary ostream (std::ostringstream, std::ofstream, ...)
		//	and override this member
		//		orbit::OrbitLogger::sErrorStream.rdbuf(my_ostream.rdbuf());
		static std::ostream& sErrorStream;
		// @brief: prints a message to a specified severity stream
		// @param severity: the severity level (see enum class OrbitLogger::Severity)
		// @param message: the message to log to a stream
		// @param function: the function that called this method
		// @param line: the line of the error
		static void print(
			Severity severity,
			std::string_view message,
			const char* function,
			unsigned line
		);
		// @brief: prints a message to a specified severity stream
		// @param severity: the severity level (see enum class OrbitLogger::Severity)
		// @param message: the message to log to a stream
		// @param result: result of a windows/directX function
		// @param function: the function that called this method
		// @param line: the line of the error
		static void print(
			Severity severity,
			std::string_view message,
			HRESULT result,
			const char* function,
			unsigned line
		);
	};

}

#define ORBIT_FUNCTION __FUNCTION__
#define ORBIT_LOG(message, severity)		\
orbit::OrbitLogger::print(					\
	severity,								\
	message,								\
	ORBIT_FUNCTION,							\
	__LINE__								\
)

#define ORBIT_LOG_HR(message, severity, hr)	\
orbit::OrbitLogger::print(					\
	severity,								\
	message,								\
	hr,										\
	ORBIT_FUNCTION,							\
	__LINE__								\
)

#define ORBIT_INFO(message) ORBIT_LOG(message, orbit::OrbitLogger::Severity::S_INFO)
#define ORBIT_INFO_HR(message, hr) ORBIT_LOG_HR(message, orbit::OrbitLogger::Severity::S_INFO, hr)
#define ORBIT_WARN(message) ORBIT_LOG(message, orbit::OrbitLogger::Severity::S_WARN)
#define ORBIT_WARN_HR(message, hr) ORBIT_LOG_HR(message, orbit::OrbitLogger::Severity::S_WARN, hr)
#define ORBIT_ERR(message) ORBIT_LOG(message, orbit::OrbitLogger::Severity::S_ERR)
#define ORBIT_ERR_HR(message, hr) ORBIT_LOG_HR(message, orbit::OrbitLogger::Severity::S_ERR, hr)

#define ORBIT_THROW(message) do { ORBIT_ERR(message); throw std::exception("See the error log for more information."); } while(false)
#define ORBIT_THROW_HR(message, hr) do { ORBIT_ERR_HR(message, hr); throw std::exception("See the error log for more information."); } while(false)

#define ORBIT_THROW_IF_FAILED(function_or_hr, message) \
do { \
	auto hr = function_or_hr; \
	if(FAILED(hr)) {		\
		ORBIT_THROW_HR(message, hr);  \
	} \
} while(false)
