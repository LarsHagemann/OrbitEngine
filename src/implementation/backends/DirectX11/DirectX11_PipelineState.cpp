#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_PipelineState.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    void DirectX11PipelineState::Bind() const
    {
        ENGINE->Context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        IPipelineState::Bind();
    }

}
#endif