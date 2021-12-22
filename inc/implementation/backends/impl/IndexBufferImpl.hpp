#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_IndexBuffer.hpp"
namespace orbit { using IndexBuffer = DirectX11IndexBuffer; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif