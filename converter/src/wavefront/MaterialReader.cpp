#include "wavefront/MaterialReader.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbtool
{

    bool WFMaterialReader::ReadFile(const fs::path& filepath, OrbIntermediate* orb)
    {
        if (!OpenFile(filepath))
            return false;

        m_orb = orb;
        // Begin
        Advance();

        while (!EndOfFile())
        {
            ExpectLiteral("newmtl");
            ParseMaterial();
        }
        
        return true;
    }

    void WFMaterialReader::ParseMaterial()
    {
        OrbMaterial material;
        auto name = Expect(TokenType::TOKEN_LITERAL).lexeme;
        material.roughness = 0.f;
        while (!MatchLiteral("newmtl") && !Match(TokenType::TOKEN_EOF))
        {
            if (MatchLiteral("Kd"))
            {
                material.diffuse.x() = std::strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                material.diffuse.y() = std::strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                material.diffuse.z() = std::strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                material.diffuse.w() = 1.f;
                if (Match(TokenType::TOKEN_NUMBER))
                    material.diffuse.w() = std::strtof(PreviousToken().lexeme.c_str(), nullptr);
            }
            else if (MatchLiteral("map_Kd"))
            {
                OrbTexture texture;
                texture.onlyReference = false;
                auto linenumber = m_currentLine;
                while (m_currentLine == linenumber)
                {
                    texture.texturePath += CurrentToken().lexeme;
                    Advance();
                }
                m_orb->AppendObject(name + "_map_Kd", texture);
                material.diffuseTextureId = name + "_map_Kd";
            }
            else
                Advance();
        }

        m_orb->AppendObject(name, material);

        if (m_pToken.type != TokenType::TOKEN_EOF)
            // Next material
            ParseMaterial();
    }

}