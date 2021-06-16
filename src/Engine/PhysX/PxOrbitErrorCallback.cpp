#include "Engine/PhysX/PxOrbitErrorCallback.hpp"
#include "Engine/Misc/Logger.hpp"

#include <cassert>

namespace orbit
{

    void PxOrbitErrorCallback::reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) 
    {
        const char* errorCode = nullptr;
        switch (code)
        {
        case PxErrorCode::eNO_ERROR:
            errorCode = "no error";
            break;
        case PxErrorCode::eINVALID_PARAMETER:
            errorCode = "invalid parameter";
            break;
        case PxErrorCode::eINVALID_OPERATION:
            errorCode = "invalid operation";
            break;
        case PxErrorCode::eOUT_OF_MEMORY:
            errorCode = "out of memory";
            break;
        case PxErrorCode::eDEBUG_INFO:
            errorCode = "info";
            break;
        case PxErrorCode::eDEBUG_WARNING:
            errorCode = "warning";
            break;
        case PxErrorCode::ePERF_WARNING:
            errorCode = "performance warning";
            break;
        case PxErrorCode::eABORT:
            errorCode = "abort";
            break;
        case PxErrorCode::eINTERNAL_ERROR:
            errorCode = "internal error";
            break;
        case PxErrorCode::eMASK_ALL:
            errorCode = "unknown error";
            break;
        }

        assert(errorCode != nullptr && "Unsupported error code.");

        Logger::print(Logger::Severity::S_ERR, FormatString("{PhysX: %s} %s", errorCode, message), file, static_cast<unsigned>(line));

        switch (code)
        {
        case PxErrorCode::eINVALID_PARAMETER:
        case PxErrorCode::eINVALID_OPERATION:
        case PxErrorCode::eOUT_OF_MEMORY:
        case PxErrorCode::eABORT:
        case PxErrorCode::eINTERNAL_ERROR:
            ORBIT_DEBUG_BREAK;
        }

        if (code == PxErrorCode::eABORT)
            throw std::exception("An unrecoverable error has occurred in Nvidia PhysX.");
    }

}