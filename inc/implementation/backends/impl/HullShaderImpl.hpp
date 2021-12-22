#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_HullShader.hpp"
namespace orbit { using HullShader = DirectX11HullShader; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif