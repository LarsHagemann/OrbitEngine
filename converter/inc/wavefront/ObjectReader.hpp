#pragma once
#include "Reader.hpp"
#include "orb/OrbIntermediate.hpp"

#include <filesystem>

namespace orbtool
{

    namespace fs = std::filesystem;

    class WFObjectReader : public Reader
    {
    private:
        bool m_warnOnQuad = true;
        bool m_triangulating = false;
        std::vector<Vector3f> m_positions;
        std::vector<Vector3f> m_normals;
        std::vector<Vector2f> m_textures;
    private:
        void ParseObject();
    public:
        void WarnOnQuads(bool warn) { m_warnOnQuad = warn; }
        bool ReadFile(const fs::path& filepath, OrbIntermediate* intermediate) override;
    };

}