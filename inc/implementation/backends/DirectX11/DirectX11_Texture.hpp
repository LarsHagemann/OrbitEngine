#pragma once
#ifdef ORBIT_DIRECTX_11
#include "interfaces/rendering/Texture.hpp"
#include "implementation/Common.hpp"

namespace orbit
{

    class DirectX11Texture : public ITextureBase<ComPtr<ID3D11Texture2D>>
    {
    private:
        ComPtr<ID3D11ShaderResourceView> m_srv;
        ComPtr<ID3D11Resource> m_texture;
    public:
        void Bind(uint32_t slot) const override;
        bool LoadImpl(std::ifstream* stream) override;
        void UnloadImpl() override;
    };

}
#endif