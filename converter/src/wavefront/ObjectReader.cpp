#include "wavefront/ObjectReader.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbtool
{

    bool WFObjectReader::ReadFile(const fs::path& filepath, OrbIntermediate* orb)
    {
        if (!OpenFile(filepath))
            return false;

        m_orb = orb;
        Advance();

        while (!EndOfFile())
        {
            if (MatchLiteral("mtllib"))
                Advance(); // Skip material libraries
            
            if (MatchLiteral("o"))
                ParseObject();
        }
        
        return true;
    }

    void WFObjectReader::ParseObject()
    {
        OrbMesh mesh;
        auto name = Expect(TokenType::TOKEN_LITERAL).lexeme;

        std::vector<OrbVertex> vertices;

        while (!Match(TokenType::TOKEN_EOF) && !MatchLiteral("o"))
        {
            if (MatchLiteral("v"))
            {
                m_positions.emplace_back(
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr),
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr),
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr)
                );
            }
            else if (MatchLiteral("vt"))
            {
                m_textures.emplace_back(
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr),
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr)
                );
            }
            else if (MatchLiteral("vn"))
            {
                m_normals.emplace_back(
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr),
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr),
                    strtof(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr)
                );
            }
            else if (MatchLiteral("f"))
            {
                uint32_t pIndices[4] = { 1, 1, 1, 1 };
                uint32_t tIndices[4] = { 1, 1, 1, 1 };
                uint32_t nIndices[4] = { 1, 1, 1, 1 };

                for (auto i = 0u; i < 3u; ++i)
                {
                    pIndices[i] = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);
                    Expect(TokenType::TOKEN_SLASH);
                    tIndices[i] = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);
                    Expect(TokenType::TOKEN_SLASH);
                    nIndices[i] = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);

                    if (pIndices[i] > m_positions.size())
                        ORBIT_THROW("Index %d out of range: [1, %d]", pIndices[i], m_positions.size());
                    if (tIndices[i] > m_textures.size())
                        ORBIT_THROW("Index %d out of range: [1, %d]", tIndices[i], m_textures.size());
                    if (nIndices[i] > m_normals.size())
                        ORBIT_THROW("Index %d out of range: [1, %d]", nIndices[i], m_normals.size());
                }

                bool is_quad = false;

                if (Match(TokenType::TOKEN_NUMBER))
                {
                    is_quad = true;
                    pIndices[3] = strtoul(PreviousToken().lexeme.c_str(), nullptr, 10);
                    Expect(TokenType::TOKEN_SLASH);
                    tIndices[3] = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);
                    Expect(TokenType::TOKEN_SLASH);
                    nIndices[3] = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);

                    if (m_warnOnQuad)
                    {
                        ORBIT_THROW("This object file contains quads. Triangulate your mesh or use -triangulate.");
                        return;
                    }
                    else if (!m_triangulating)
                    {
                        m_triangulating = true;
                        ORBIT_LOG("Naively triangulating mesh.");
                    }
                }

                std::vector<uint32_t> tris = { 0, 1, 2 };
                if (is_quad)
                    tris = { 0, 1, 2, 0, 2, 3 };

                for (auto idx : tris)
                {
                    vertices.emplace_back(OrbVertex{ m_positions[pIndices[idx] - 1], m_normals[nIndices[idx] - 1], Vector3f{}, m_textures[tIndices[idx] - 1] });
                }
            }
            else if (MatchLiteral("usemtl"))
            {
                mesh.material = Expect(TokenType::TOKEN_LITERAL).lexeme;
            }
            else Advance();
        }

        // Postprocess

        std::vector<uint32_t> indices;
        std::vector<OrbVertex> mesh_vertices;

        for (const auto& vertex : vertices)
        {
            auto it = std::find(mesh_vertices.begin(), mesh_vertices.end(), vertex);
            if (it == mesh_vertices.end())
            {
                indices.emplace_back(static_cast<uint32_t>(mesh_vertices.size()));
                mesh_vertices.emplace_back(vertex);
            }
            else
            {
                uint32_t index = it - mesh_vertices.begin();
                indices.emplace_back(index);
            }
        }

        if (mesh_vertices.size() == indices.size())
            indices.clear();

        SubMesh sMesh;
        sMesh.startIndex = 0;
        sMesh.startVertex = 0;
        sMesh.indexCount = indices.size();
        sMesh.vertexCount = mesh_vertices.size();

        mesh.vertices = std::move(mesh_vertices);
        mesh.indices = std::move(indices);
        mesh.submeshes.emplace_back(sMesh);
        m_orb->AppendObject(name, mesh);

        if (PreviousToken().type != TokenType::TOKEN_EOF)
            ParseObject();
    }

}