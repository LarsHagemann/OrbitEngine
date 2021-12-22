#pragma once
#include <cstdint>

namespace orbit
{

    class Color
    {
    private:
        float m_raw[4];
    public:
        constexpr Color(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0) :
            m_raw{ red / 255.f, green / 255.f, blue / 255.f, alpha / 255.f }
        {}
        float red()   const { return m_raw[0]; }
        float green() const { return m_raw[1]; }
        float blue()  const { return m_raw[2]; }
        float alpha() const { return m_raw[3]; }
        void fromRGBA(uint8_t red = 0, uint8_t green = 0, uint8_t blue = 0, uint8_t alpha = 0)
        {
            m_raw[0] = red / 255.f;
            m_raw[1] = green / 255.f;
            m_raw[2] = blue / 255.f;
            m_raw[3] = alpha / 255.f;
        }
        void red(uint8_t red)
        {
            m_raw[0] = red / 255.f;
        }
        void green(uint8_t green)
        {
            m_raw[1] = green / 255.f;
        }
        void blue(uint8_t blue)
        {
            m_raw[2] = blue / 255.f;
        }
        void alpha(uint8_t alpha)
        {
            m_raw[3] = alpha / 255.f;
        }
    };

    namespace Colors {
        static constexpr auto White  = Color{ 255, 255, 255, 255 };
        static constexpr auto Black  = Color{ 0, 0, 0, 255 };
        static constexpr auto Red    = Color{ 255, 0, 0, 255 };
        static constexpr auto Green  = Color{ 0, 255, 0, 255 };
        static constexpr auto Blue   = Color{ 0, 0, 255, 255 };
        static constexpr auto Yellow = Color{ 255, 255, 0, 255 };
        static constexpr auto Violet = Color{ 255, 0, 255, 255 };
        static constexpr auto Cyan   = Color{ 0, 255, 255, 255 };
    }

    using Albedo = Color;
    using Specular = Color;

}