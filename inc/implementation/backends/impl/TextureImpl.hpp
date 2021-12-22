#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_Texture.hpp"
namespace orbit { using Texture = DirectX11Texture; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif