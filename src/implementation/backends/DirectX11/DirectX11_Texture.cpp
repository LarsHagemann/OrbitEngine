#ifdef ORBIT_DIRECTX_11
#include "implementation/backends/DirectX11/DirectX11_Texture.hpp"
#include "implementation/misc/DDSTextureLoader.h"
#include "implementation/misc/WICTextureLoader.h"
#include "implementation/engine/Engine.hpp"

namespace orbit
{

    void DirectX11Texture::Bind(uint32_t slot) const
    {
#ifdef _DEBUG
        if (m_srv == nullptr)
        {
            ORBIT_ERROR("You tried to bind an empty texture");
            return;
        }
#endif
        ENGINE->Context()->PSSetShaderResources(slot, 1, m_srv.GetAddressOf());
    }

    bool DirectX11Texture::LoadImpl(std::ifstream* stream) 
    {
        auto type = ENGINE->RMGetResourceType(GetId());
        if (type == ResourceType::TEXTURE)
        {
            uint64_t textureSize = 0u;
            std::vector<uint8_t> binary;
            stream->read((char*)&textureSize, sizeof(uint64_t));
            binary.resize(textureSize);
            stream->read((char*)binary.data(), textureSize);
            auto result = DirectX::CreateWICTextureFromMemory(
                ENGINE->Device().Get(),
                ENGINE->Context().Get(),
                (uint8_t*)binary.data(),
                binary.size(),
                nullptr,
                m_srv.ReleaseAndGetAddressOf()
            );
            if (FAILED(result))
            {
                ORBIT_ERROR_HR(result, "Unable to load texture %lld", GetId());
                result = DirectX::CreateDDSTextureFromMemory(
                    ENGINE->Device().Get(),
                    ENGINE->Context().Get(),
                    (uint8_t*)binary.data(),
                    binary.size(),
                    nullptr,
                    m_srv.ReleaseAndGetAddressOf()
                );
                if (FAILED(result))
                    ORBIT_ERROR_HR(result, "Unable to load texture %lld", GetId());
            }
        }
        else if (type == ResourceType::TEXTURE_REFERENCE)
        {
            uint32_t pathLen = 0u;
            stream->read((char*)&pathLen, sizeof(uint32_t));
            std::string path;
            path.resize(pathLen);
            stream->read(path.data(), pathLen);
            auto ppath = fs::path(path);
            auto result = DirectX::CreateWICTextureFromFile(
                ENGINE->Device().Get(),
                ENGINE->Context().Get(),
                ppath.c_str(),
                nullptr,
                m_srv.ReleaseAndGetAddressOf()
            );
            if (FAILED(result))
            {
                ORBIT_ERROR_HR(result, "Unable to load texture %lld", GetId());
                result = DirectX::CreateDDSTextureFromFile(
                    ENGINE->Device().Get(),
                    ENGINE->Context().Get(),
                    ppath.c_str(),
                    nullptr,
                    m_srv.ReleaseAndGetAddressOf()
                );
                if (FAILED(result))
                    ORBIT_ERROR_HR(result, "Unable to load texture %lld", GetId());
            }
        }
        return true;
    }

    void DirectX11Texture::UnloadImpl()
    {
        m_textureContainer = nullptr;
        m_srv = nullptr;
    }

}
#endif