#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_DomainShader.hpp"
namespace orbit { using DomainShader = DirectX11DomainShader; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif