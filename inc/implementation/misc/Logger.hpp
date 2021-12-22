#pragma once
#include <ostream>
#include <ctime>
#include <mutex>

#include "implementation/misc/Helper.hpp"
#include "interfaces/misc/Loggable.hpp"

#ifndef ORBIT_DEBUG_LEVEL
#ifdef _DEBUG
#define ORBIT_DEBUG_LEVEL 5
#else
#define ORBIT_DEBUG_LEVEL 3
#endif
#define ORBIT_LEVEL_DEBUG 4
#define ORBIT_LEVEL_MISC  3
#define ORBIT_LEVEL_WARN  2
#define ORBIT_LEVEL_ERROR 1
#endif

#ifdef ORBIT_WINDOWS
#include <Windows.h>
#endif

namespace orbit
{

    // The orbit Logging utility. For ease of use it is recommended to use the macros
    // defined at the end of the file. This utility implements formatted output to various streams
    // with additional information for "Loggable" (@see inc/interfaces/misc/Loggable.hpp) objects.
    class Logger
    {
    public:
        // @member: stream to write debugging information to
        static std::ostream& infoStream;
        // @member: stream to write error information to
        static std::ostream& errorStream;

        // @member: mutex for writing from different threads
        static std::mutex sPrintingMutex;
    private:
        // @method: prints data to the specified stream
        // @param stream: The stream to write to
        // @param format: format string for formatted output.
        // @param function_name: The callee's function name (usually __FUNCTION__)
        // @param linenumber: The callee's linenumber (usually __LINE__)
        // @param ts: parameters to the formatted string
        // @see Log(...)
        // @see Error(...)
        template<class... Ts>
        static void Print(std::ostream& stream, const char* format, const char* function_name, int linenumber, Ts&&...ts)
        {
            std::time_t t = std::time(0);
            std::tm now;
            localtime_s(&now, &t);

            std::scoped_lock<std::mutex> lock(sPrintingMutex);
            stream << '[' << (now.tm_year + 1900) << '-'
                << (now.tm_mon + 1) << '-'
                << now.tm_mday
                << "  " << now.tm_hour
                << ':' << now.tm_min
                << ':' << now.tm_sec
                << "] ";
            stream << string_print(format, ts...);
            stream << "  --  <in " << function_name << ':' << linenumber << '>' << '\n';
        }
    public:
        // @method: Prints debugging information to the info stream
        // @param format: format string for formatted output.
        // @param function_name: The callee's function name (usually __FUNCTION__)
        // @param linenumber: The callee's linenumber (usually __LINE__)
        // @param ts: parameters to the formatted string
        // @example:
        //  orb::Logger::Log("Helpful information: %s", __FUNCTION__, __LINE__, "information");
        //  // or using the macro:
        //  ORBIT_LOG("Helpful information: %s", "information");
        template<class... Ts>
        static void Log(const char* format, const char* function_name, int linenumber, Ts&&...ts)
        {
            infoStream << "[INFO]";
            Print(infoStream, format, function_name, linenumber, ts...);
        }
        // @method: Prints a loggable object to the info stream
        // @param loggable: The object to be logged
        static void Log(const Loggable& loggable)
        {
            loggable.Log(infoStream);
        }
        // @method: Prints debugging information to the info stream
        // @param format: format string for formatted output.
        // @param function_name: The callee's function name (usually __FUNCTION__)
        // @param linenumber: The callee's linenumber (usually __LINE__)
        // @param ts: parameters to the formatted string
        // @example:
        //  orb::Logger::Error("Helpful information: %s", __FUNCTION__, __LINE__, "information");
        //  // or using the macro:
        //  ORBIT_ERROR("Helpful information: %s", "information");
        template<class... Ts>
        static void Error(const char* format, const char* function_name, int linenumber, Ts&&...ts)
        {
            errorStream << "[ERROR]";
            Print(errorStream, format, function_name, linenumber, ts...);
        }
        // @method: Prints a loggable object to the error stream
        // @param loggable: The object to be logged
        static void Error(const Loggable& loggable)
        {
            loggable.Log(errorStream);
        }

#ifdef ORBIT_WINDOWS
    private:
        // @method: prints data to the specified stream
        // @param stream: The stream to write to
        // @param format: format string for formatted output.
        // @param function_name: The callee's function name (usually __FUNCTION__)
        // @param linenumber: The callee's linenumber (usually __LINE__)
        // @param ts: parameters to the formatted string
        // @see Log(...)
        // @see Error(...)
        template<class... Ts>
        static void Print(HRESULT hr, std::ostream& stream, const char* format, const char* function_name, int linenumber, Ts&&...ts)
        {
            std::time_t t = std::time(0);
            std::tm now;
            localtime_s(&now, &t);

            std::scoped_lock<std::mutex> lock(sPrintingMutex);
            stream << '[' << (now.tm_year + 1900) << '-'
                << (now.tm_mon + 1) << '-'
                << now.tm_mday
                << "  " << now.tm_hour
                << ':' << now.tm_min
                << ':' << now.tm_sec
                << "] ";

            stream << '{' << std::system_category().message(hr) << "} ";
            stream << string_print(format, ts...);
            stream << "  --  <in " << function_name << ':' << linenumber << '>' << '\n';
        }
    public:
        // @method: Prints debugging information to the info stream
        // @param hr: The HRESULT that should be logged (for example because it failed)
        // @param format: format string for formatted output.
        // @param function_name: The callee's function name (usually __FUNCTION__)
        // @param linenumber: The callee's linenumber (usually __LINE__)
        // @param ts: parameters to the formatted string
        // @example:
        //  orb::Logger::Log("Helpful information: %s", __FUNCTION__, __LINE__, "information");
        //  // or using the macro:
        //  ORBIT_LOG("Helpful information: %s", "information");
        template<class... Ts>
        static void Log(HRESULT hr, const char* format, const char* function_name, int linenumber, Ts&&...ts)
        {
            infoStream << "[INFO]";
            Print(hr, infoStream, format, function_name, linenumber, ts...);
        }
        // @method: Prints debugging information to the info stream
        // @param hr: The HRESULT that should be logged (for example because it failed)
        // @param format: format string for formatted output.
        // @param function_name: The callee's function name (usually __FUNCTION__)
        // @param linenumber: The callee's linenumber (usually __LINE__)
        // @param ts: parameters to the formatted string
        // @example:
        //  orb::Logger::Error("Helpful information: %s", __FUNCTION__, __LINE__, "information");
        //  // or using the macro:
        //  ORBIT_ERROR("Helpful information: %s", "information");
        template<class... Ts>
        static void Error(HRESULT hr, const char* format, const char* function_name, int linenumber, Ts&&...ts)
        {
            errorStream << "[ERROR]";
            Print(hr, errorStream, format, function_name, linenumber, ts...);
        }
#endif
    };

// Helper macro for logging information to the info stream
//  ORBIT_LOG("Helpful information: %s", "information");
#define ORBIT_LOG(format, ...) ::orbit::Logger::Log(format, __FUNCTION__, __LINE__, __VA_ARGS__)
// Helper macro for logging information to the error stream
//  ORBIT_ERROR("Helpful information: %s", "information");
#define ORBIT_ERROR(format, ...) ::orbit::Logger::Error(format, __FUNCTION__, __LINE__, __VA_ARGS__)

#define ORBIT_THROW(format, ...) do { ::orbit::Logger::Error(format, __FUNCTION__, __LINE__, __VA_ARGS__); throw std::exception("Exception occurred. See log for more details."); } while(false)
// Helper macro for logging OS-error codes
#ifdef ORBIT_WINDOWS
#define ORBIT_LOG_HR(function_or_hresult, format, ...) ::orbit::Logger::Log(function_or_hresult, format, __FUNCTION__, __LINE__, __VA_ARGS__)
#define ORBIT_ERROR_HR(function_or_hresult, format, ...) ::orbit::Logger::Error(function_or_hresult, format, __FUNCTION__, __LINE__, __VA_ARGS__)

#define ORBIT_THROW_HR(function_or_hr, format, ...) do { ::orbit::Logger::Error(function_or_hr, format, __FUNCTION__, __LINE__, __VA_ARGS__); throw std::exception("Exception occurred. See log for more details."); } while(false)

#define ORBIT_THROW_IF_FAILED(function_or_hr, format, ...)\
do { \
    auto orbit_macro_hr = function_or_hr; \
    if (FAILED(orbit_macro_hr)) { \
        ORBIT_THROW_HR(orbit_macro_hr, format, __VA_ARGS__); \
    } \
} while(false)
#endif

#define ORBIT_INFO_LEVEL(level, format, ...)  \
if constexpr (level < ORBIT_DEBUG_LEVEL) { \
    ORBIT_LOG(format, __VA_ARGS__); \
}

}
