#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "interfaces/rendering/PipelineState.hpp"

namespace orbit
{

    class DirectX11PipelineState : public IPipelineState
    {
    private:
        D3D11_PRIMITIVE_TOPOLOGY m_topology;
    public:
        void Bind() const override;
    };

}
#endif