#pragma once
#include <cstdint>

namespace orbit
{

    using ResourceId = uint64_t;

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