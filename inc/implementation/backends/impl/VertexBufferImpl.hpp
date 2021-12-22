#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_VertexBuffer.hpp"
namespace orbit {
    template<typename Vertex>
    using VertexBuffer = DirectX11VertexBuffer<Vertex>;
}
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif