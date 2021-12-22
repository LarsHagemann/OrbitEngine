#pragma once
#include "implementation/Common.hpp"

namespace orbit
{

    struct Submesh
    {
        size_t startVertex = 0u;
		size_t vertexCount = 0u;
		size_t startIndex = 0u;
		size_t indexCount = 0u;
        ResourceId materialId;
        ResourceId pipelineStateId;
    };

}