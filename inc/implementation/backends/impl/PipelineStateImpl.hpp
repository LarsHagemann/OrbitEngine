#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_PipelineState.hpp"
namespace orbit { using PipelineState = DirectX11PipelineState; }
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif