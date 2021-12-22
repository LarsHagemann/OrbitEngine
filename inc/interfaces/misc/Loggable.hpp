#pragma once
#include <ostream>

namespace orbit
{

    // Provides an interface for classes that may want to log additional information 
    // I.e. for debugging purposes only printing the OrbID is vague. But if the registry pulls
    // an object you can use its <Log()> function to show additional data. A texture might
    // then tell you its source file and dimensions, a material its name and so on.
    class Loggable
    {   
    protected:
        void PrintName(std::ostream& stream, const char* name) const
        {
            stream << "--- <" << name << "> ---\n";
        }
        template<class...Ts>
        void PrintInfo(std::ostream& stream, const char* format, Ts...ts) const
        {
            stream << "  - ";
            stream << string_print(format, ts...) << '\n';
        }
    public:
        virtual void Log(std::ostream& stream) const
        {
            stream << "--- <Unknown Object> ---\n";
        }
    };

    static std::ostream& operator<<(std::ostream& stream, const Loggable& loggable)
    {
        loggable.Log(stream);
        return stream;
    }

}