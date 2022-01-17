#include "interfaces/rendering/Material.hpp"
#include "interfaces/engine/EngineBase.hpp"

#include "implementation/engine/Engine.hpp"
#include "implementation/backends/impl/TextureImpl.hpp"

namespace orbit
{

    void MaterialBase::UnloadImpl()
    {
        // unloaded...
        m_buffer = nullptr;
    }

    bool MaterialBase::LoadImpl(std::ifstream* stream)
    {
        m_buffer = std::make_unique<BufferType>();
        stream->read((char*)m_buffer->GetPointerToObject<0>(), sizeof(Vector4f));
        stream->read((char*)m_buffer->GetPointerToObject<1>(), sizeof(Vector4f));
        stream->read((char*)m_buffer->GetPointerToObject<2>(), sizeof(float));
        stream->read((char*)m_buffer->GetPointerToObject<3>(), sizeof(MaterialFlags));
        
        m_albedoTexture = ReadReferenceId(stream);
        m_normalTexture = ReadReferenceId(stream);
        m_roughnessTexture = ReadReferenceId(stream);
        m_occlusionTexture = ReadReferenceId(stream);

        *m_buffer->GetPointerToObject<3>() = 
            (m_albedoTexture != GetId() ? static_cast<uint32_t>(MaterialFlag::FLAG_HAS_ALBEDO_TEXTURE) : 0) |
            (m_normalTexture != GetId() ? static_cast<uint32_t>(MaterialFlag::FLAG_HAS_NORMAL_MAP) : 0) |
            (m_roughnessTexture != GetId() ? static_cast<uint32_t>(MaterialFlag::FLAG_HAS_ROUGHNESS_TEXTURE) : 0) |
            (m_occlusionTexture != GetId() ? static_cast<uint32_t>(MaterialFlag::FLAG_HAS_OCCLUSION_MAP) : 0);
        m_buffer->UpdateBuffer();
        return true;
    }

    void MaterialBase::Bind(uint32_t slot) const
    {
#ifdef _DEBUG
        if (!m_buffer)
        {
            ORBIT_ERROR("You are trying to bind an empty material");
            return;
        }
#endif
        m_buffer->BindBuffer(slot, BindShaderType::PixelShader);

        // Bind Textures
        if (m_albedoTexture != GetId())
            ENGINE->RMLoadResource<Texture>(m_albedoTexture)->Bind(1);
        if (m_normalTexture != GetId())
            ENGINE->RMLoadResource<Texture>(m_normalTexture)->Bind(2);
        if (m_occlusionTexture != GetId())
            ENGINE->RMLoadResource<Texture>(m_occlusionTexture)->Bind(3);
        if (m_roughnessTexture != GetId())
            ENGINE->RMLoadResource<Texture>(m_roughnessTexture)->Bind(4);
    }

}