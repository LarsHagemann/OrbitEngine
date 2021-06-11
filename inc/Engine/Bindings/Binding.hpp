#pragma once
#include "Engine/Bindings/Platform.hpp"

#ifdef ORBIT_WIN
#include "Engine/Bindings/Windows/WindowWin.hpp"
#elif defined ORBIT_UNIX
#include "Engine/Bindings/Windows/UnixWin.hpp"
#endif

#ifdef ORBIT_DX12
#include "Engine/Bindings/DX/COM.hpp"
#include "Engine/Bindings/DX/DXGIHelper.hpp"
#include "Engine/Bindings/DX/DXInput.hpp"
#include "Engine/Bindings/DX12/CommandQueue.hpp"
#include "Engine/Bindings/DX12/DX12Engine.hpp"
#include "Engine/Bindings/DX12/DX12Helper.hpp"
#include "Engine/Bindings/DX12/DX12Renderer.hpp"
#include "Engine/Bindings/DX12/UploadBuffer.hpp"
#include "Engine/Bindings/DX12/DX12Buffer.hpp"

#elif defined ORBIT_DX11
#include "Engine/Bindings/DX"
#include "Engine/Bindings/DX11"

namespace orbit
{
    using VertexBuffer  = Ptr<ID3D11Buffer>;
    using IndexBuffer   = Ptr<ID3D11Buffer>;
    using ShaderBuffer  = Ptr<ID3D11ShaderResourceView>;
    using TextureBuffer = Ptr<ID3D11Buffer>;
}
#elif defined ORBIT_OPENGL

#endif
