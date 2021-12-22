#pragma once
#ifdef ORBIT_DIRECTX_11
#include "implementation/Common.hpp"

#include "interfaces/rendering/IndexBuffer.hpp"

namespace orbit
{

    class DirectX11IndexBuffer : public IIndexBufferBase<ComPtr<ID3D11Buffer>>
    {
    public:
        void UpdateBuffer() override;
        void Bind(uint32_t offset) const override;
    };

}
#endif