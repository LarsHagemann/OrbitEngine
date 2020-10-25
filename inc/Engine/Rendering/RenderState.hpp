#pragma once

#ifdef ORBIT_DX12
#include <d3d12.h>
#include <d3dx12.h>
#elif defined ORBIT_DX11
#include <d3d11.h>
#include <D3DX11.h>
#elif defined ORBIT_OPENGL
#include OPENGL_ERROR_INCLUDE
#endif

namespace orbit
{

    // @brief: empty at the moment
    struct RenderState
    {
#ifdef ORBIT_DX12
        static constexpr auto sEmptyViewport = D3D12_VIEWPORT{ 0, 0, 0, 0, 0, 0 };
        static constexpr auto sEmptyRect = D3D12_RECT{ 0, 0, 0, 0 };
        D3D12_VIEWPORT viewport = D3D12_VIEWPORT{ 0, 0, 0, 0, 0, 0 };
        D3D12_RECT scissorRect = D3D12_RECT{ 0, 0, 0, 0 };
#elif defined ORBIT_DX11
        D3D11_VIEWPORT viewport;
        D3D11_RECT scissorRect;
#elif defined ORBIT_OPENGL
        OPENGL_ERROR_TYPE viewport;
        OPENGL_ERROR_TYPE scissorRect;
#endif
    };

}
