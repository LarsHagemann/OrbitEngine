#pragma once
#include "implementation/engine/ResourceType.hpp"

#include <string>

namespace orbit
{

    struct ResourceHeader
    {
        uint64_t     id;
        ResourceType type;
        uint32_t     payloadSize;
        std::string  name;
    };

}