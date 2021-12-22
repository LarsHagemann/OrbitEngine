#include <cstdint>

namespace orbit
{

    class Version
    {
    public:
        uint32_t m_version;
    private:
        static constexpr auto sPatchMask = 0x00000FFF;
        static constexpr auto sMinorMask = 0x00FFF000;
        static constexpr auto sMajorMask = 0xFF000000;
    public:
        constexpr Version(uint64_t version) :
            m_version(version)
        {}
        constexpr Version(uint8_t major, uint16_t minor, uint16_t patch) :
            m_version(
                (static_cast<uint64_t>(major) << 24 & sMajorMask) | 
                (static_cast<uint64_t>(minor) << 12 & sMinorMask) | 
                (static_cast<uint64_t>(patch) << 0  & sPatchMask))
        {}
        uint16_t patch() const
        {
            return static_cast<uint16_t>((m_version & sPatchMask) >> 0);
        }
        uint16_t minor() const
        {
            return static_cast<uint16_t>((m_version & sMinorMask) >> 12);
        }
        uint8_t major() const
        {
            return static_cast<uint8_t>((m_version & sMajorMask) >> 24);
        }
    };

    static bool operator<(Version left, Version right)
    {
        return left.m_version < right.m_version;
    }
    static bool operator<=(Version left, Version right)
    {
        return left.m_version <= right.m_version;
    }
    static bool operator>(Version left, Version right)
    {
        return left.m_version > right.m_version;
    }
    static bool operator>=(Version left, Version right)
    {
        return left.m_version >= right.m_version;
    }
    static bool operator==(Version left, Version right)
    {
        return left.m_version == right.m_version;
    }
    static bool operator!=(Version left, Version right)
    {
        return left.m_version != right.m_version;
    }

}