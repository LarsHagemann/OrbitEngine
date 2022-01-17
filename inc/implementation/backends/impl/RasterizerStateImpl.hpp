#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_RasterizerState.hpp"
namespace orbit { using RasterizerState = DirectX11RasterizerState; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif