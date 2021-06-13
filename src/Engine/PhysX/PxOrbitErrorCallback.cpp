#include "Engine/PhysX/PxOrbitErrorCallback.hpp"
#include "Engine/Misc/Logger.hpp"

namespace orbit
{

    void PxOrbitErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) 
    {
        Logger::print(Logger::Severity::S_ERR, FormatString("{PhysX:} %s", message), file, static_cast<unsigned>(line));
    }

}