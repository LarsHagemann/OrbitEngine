#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_InputLayout.hpp"
namespace orbit { using InputLayout = DirectX11InputLayout; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif