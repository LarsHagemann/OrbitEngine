#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_ConstantBuffer.hpp"
namespace orbit { 
    template<typename...Ts>
    using ConstantBuffer = DirectX11ConstantBuffer<Ts...>;
}
#elif defined ORBIT_DIRECTX_12

#elif defined ORBIT_OPENGL

#endif