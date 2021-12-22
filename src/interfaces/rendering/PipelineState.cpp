#include "interfaces/rendering/PipelineState.hpp"
#include "implementation/engine/Engine.hpp"
#include "implementation/backends/Platform.hpp"

namespace orbit
{

    void IPipelineState::Bind() const
    {
        if (m_vertexShaderId != GetId())
            ENGINE->RMLoadResource<VertexShader>(m_vertexShaderId)->Bind();
        if (m_pixelShaderId != GetId())
            ENGINE->RMLoadResource<PixelShader>(m_pixelShaderId)->Bind();
        if (m_inputLayoutId != GetId())
            ENGINE->RMLoadResource<InputLayout>(m_inputLayoutId)->Bind();
    }

    bool IPipelineState::LoadImpl(std::ifstream* stream)
    {
        int64_t
            vs_offset = 0u,
            ps_offset = 0u,
            gs_offset = 0u,
            ds_offset = 0u,
            hs_offset = 0u,
            il_offset = 0u,
            rs_offset = 0u,
            bs_offset = 0u;
        stream->read((char*)&vs_offset, sizeof(int64_t));
        stream->read((char*)&ps_offset, sizeof(int64_t));
        stream->read((char*)&gs_offset, sizeof(int64_t));
        stream->read((char*)&ds_offset, sizeof(int64_t));
        stream->read((char*)&hs_offset, sizeof(int64_t));
        stream->read((char*)&il_offset, sizeof(int64_t));
        stream->read((char*)&rs_offset, sizeof(int64_t));
        stream->read((char*)&bs_offset, sizeof(int64_t));
        m_vertexShaderId = GetId() + vs_offset;
        m_pixelShaderId  = GetId() + ps_offset;
        m_geometryShaderId  = GetId() + gs_offset;
        m_domainShaderId  = GetId() + ds_offset;
        m_hullShaderId  = GetId() + hs_offset;
        m_inputLayoutId  = GetId() + il_offset;
        m_rasterizerStateId  = GetId() + rs_offset;
        m_blendStateId  = GetId() + bs_offset;
        return true;
    }
    
    void IPipelineState::UnloadImpl()
    {
        m_vertexShaderId = GetId();
        m_pixelShaderId = GetId();
        m_geometryShaderId = GetId();
        m_domainShaderId = GetId();
        m_hullShaderId = GetId();
        m_inputLayoutId = GetId();
        m_rasterizerStateId = GetId();
        m_blendStateId = GetId();
    }

}