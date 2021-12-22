#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_GeometryShader.hpp"
namespace orbit { using GeometryShader = DirectX11GeometryShader; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif