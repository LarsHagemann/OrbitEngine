#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_VertexShader.hpp"
namespace orbit { using VertexShader = DirectX11VertexShader; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif