#pragma once

#include <foundation/PxErrorCallback.h>
#include <PxPhysXConfig.h>

namespace orbit
{

    using namespace physx;

    class PxOrbitErrorCallback : public PxErrorCallback
    {
    public:

        virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override;
    };

}