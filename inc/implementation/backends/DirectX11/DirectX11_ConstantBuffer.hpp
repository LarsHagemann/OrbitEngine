#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"
#include "implementation/backends/impl/EngineImpl.hpp"
#include "implementation/engine/Engine.hpp"
#include "interfaces/misc/ConstantBuffer.hpp"

namespace orbit
{

    template<typename...Ts>
    class DirectX11ConstantBuffer : public IConstantBufferBase<Ts...>
    {
    private:
        ComPtr<ID3D11Buffer> m_bufferHandle;
    public:
        DirectX11ConstantBuffer() 
        {
        }
        virtual void UpdateBuffer() override
        {
            if (!m_bufferHandle)
            {
                D3D11_BUFFER_DESC desc;
                ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
                desc.Usage = D3D11_USAGE_DEFAULT;
                desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
                desc.ByteWidth = Math<uint32_t>::AlignUp(buffer_size, 16);

                D3D11_SUBRESOURCE_DATA subresource;
                ZeroMemory(&subresource, sizeof(D3D11_SUBRESOURCE_DATA));
                subresource.pSysMem = this->GetBufferPointer();

                auto result = ENGINE->Device()->CreateBuffer(
                    &desc,
                    &subresource,
                    m_bufferHandle.ReleaseAndGetAddressOf()
                );
                if (FAILED(result))
                    ORBIT_ERROR_HR(result, "Failed to create buffer");
            }
            else
            {
                D3D11_MAPPED_SUBRESOURCE mappedResource;
                ENGINE->Context()->UpdateSubresource(m_bufferHandle.Get(), 0, nullptr, GetBufferPointer(), buffer_size, 0);
            }
        }
        virtual void BindBuffer(ShaderIndex index, BitField<8> shaders = { BindShaderType::VertexShader, BindShaderType::PixelShader }) const override
        {
            if (shaders.test(BindShaderType::VertexShader))
                ENGINE->Context()->VSSetConstantBuffers(index, 1, m_bufferHandle.GetAddressOf());
            if (shaders.test(BindShaderType::PixelShader))
                ENGINE->Context()->PSSetConstantBuffers(index, 1, m_bufferHandle.GetAddressOf());
            if (shaders.test(BindShaderType::GeometryShader))
                ;//Engine::Context()->GSSetConstantBuffers(index, 1, m_bufferHandle.GetAdressOf());
        }
    };

}

#endif