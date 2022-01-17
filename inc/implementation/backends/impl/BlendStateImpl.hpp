#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_BlendState.hpp"
namespace orbit { using BlendState = DirectX11BlendState; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif