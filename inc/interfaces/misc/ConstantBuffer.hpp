#pragma once
#include "implementation/Common.hpp"
#include "implementation/misc/HeapBufferView.hpp"
#include "implementation/misc/BitField.hpp"

namespace orbit
{

    enum class BindShaderType : uint8_t
    {
        VertexShader   = 1 << 0,
        PixelShader    = 1 << 1,
        GeometryShader = 1 << 2
    };

    template<typename...Ts>
    class IConstantBufferBase : public HeapBufferView<Ts...>
    {
    public:
        virtual void BindBuffer(ShaderIndex index, BitField<8> shaders = { BindShaderType::VertexShader, BindShaderType::PixelShader }) const = 0;
        virtual void UpdateBuffer() = 0;
    };

}