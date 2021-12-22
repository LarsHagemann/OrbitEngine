#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_PixelShader.hpp"
namespace orbit { using PixelShader = DirectX11PixelShader; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif