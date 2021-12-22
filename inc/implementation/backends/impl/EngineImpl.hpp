#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_Engine.hpp"
namespace orbit {
    using Engine = DirectX11Engine;
    using EngineInitDesc = DirectX11Engine_Desc;
}
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif