#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_SamplerState.hpp"
namespace orbit { using SamplerState = DirectX11SamplerState; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif