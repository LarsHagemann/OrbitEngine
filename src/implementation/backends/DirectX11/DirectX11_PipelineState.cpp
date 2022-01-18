#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_PipelineState.hpp"
#include "implementation/misc/PrimitiveType.hpp"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    bool DirectX11PipelineState::LoadImpl(std::ifstream* stream)
    {
        m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        EPrimitiveType type;
        stream->read((char*)&type, 1);

        switch (type)
        {
        case orbit::TRIANGLES:
            m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            break;
        case orbit::TRIANGLE_STRIP:
            m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            break;
        case orbit::TRIANGLE_FAN:
            // not supported at the moment
            //m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLEFAN;
            ORBIT_ERROR("Primitive topology 'TRIANGLE_FAN' not supported at the moment.");
            break;
        case orbit::LINES:
            m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            break;
        case orbit::POINTS:
            m_topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            break;
        default:
            break;
        }

        return IPipelineState::LoadImpl(stream);
    }

    void DirectX11PipelineState::Bind() const
    {
        ENGINE->Context()->IASetPrimitiveTopology(m_topology);
        IPipelineState::Bind();
    }

}
#endif