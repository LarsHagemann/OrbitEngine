#include "interfaces/rendering/PipelineState.hpp"
#include "implementation/engine/Engine.hpp"
#include "implementation/backends/Platform.hpp"

namespace orbit
{

    void IPipelineState::Bind() const
    {
        if (IsValidId(m_vertexShaderId))
            ENGINE->RMLoadResource<VertexShader>(m_vertexShaderId)->Bind();
        if (IsValidId(m_pixelShaderId))
            ENGINE->RMLoadResource<PixelShader>(m_pixelShaderId)->Bind();
        if (IsValidId(m_geometryShaderId))
            ENGINE->RMLoadResource<GeometryShader>(m_geometryShaderId)->Bind();
        if (IsValidId(m_domainShaderId))
            ENGINE->RMLoadResource<DomainShader>(m_domainShaderId)->Bind();
        if (IsValidId(m_hullShaderId))
            ENGINE->RMLoadResource<HullShader>(m_hullShaderId)->Bind();
        //if (IsValidId(m_blendStateId))
        //    ENGINE->RMLoadResource<BlendState>(m_blendStateId)->Bind();
        for (const auto&[slot,samplerId] : m_samplerStateIds)
        {
            if (IsValidId(samplerId))
                ENGINE->RMLoadResource<SamplerState>(samplerId)->Bind(slot);
        }
        if (IsValidId(m_rasterizerStateId))
            ENGINE->RMLoadResource<RasterizerState>(m_rasterizerStateId)->Bind();
        //if (IsValidId(m_rootSignatureId))
        //    ENGINE->RMLoadResource<RootSignature>(m_rootSignatureId)->Bind();
        if (IsValidId(m_inputLayoutId))
            ENGINE->RMLoadResource<InputLayout>(m_inputLayoutId)->Bind();
    }

    bool IPipelineState::LoadImpl(std::ifstream* stream)
    {
        m_vertexShaderId = ReadReferenceId(stream);
        m_pixelShaderId  = ReadReferenceId(stream);
        m_geometryShaderId  = ReadReferenceId(stream);
        m_domainShaderId  = ReadReferenceId(stream);
        m_hullShaderId  = ReadReferenceId(stream);
        m_inputLayoutId  = ReadReferenceId(stream);
        m_rasterizerStateId  = ReadReferenceId(stream);
        m_blendStateId  = ReadReferenceId(stream);
        uint8_t numSamplers = 0;
        stream->read((char*)&numSamplers, 1);
        for (auto i = 0u; i < numSamplers; ++i)
        {
            uint32_t slot;
            stream->read((char*)&slot, sizeof(uint32_t));
            auto samplerId = ReadReferenceId(stream);
            m_samplerStateIds.emplace(slot, samplerId);
        }
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
        m_samplerStateIds.clear();
    }

}