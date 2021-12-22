#pragma once
#include "orb/OrbIntermediate.hpp"
#include "Reader.hpp"

#include <filesystem>

namespace orbtool
{

    namespace fs = std::filesystem;

    class RawReader : public Reader
    {
    private:
        void BeginBlock();
        void EndBlock();
        bool ReadDirective();
        void Read_ReadDirective();
        void Read_NewDirective();
        void Read_Texture(const fs::path& path);
        void Read_ShaderCode(const fs::path& path, ShaderType shader_type);
        void Read_Shader(const fs::path& path, ShaderType shader_type);
        OrbBlendState Read_BlendState();
        OrbInputLayout Read_InputLayout();
        OrbPipelineState Read_PipelineState();
        OrbRasterizerState Read_RasterizerState();
        OrbMaterial Read_Material();
    public:
        bool ReadFile(const fs::path& filepath, OrbIntermediate* intermediate) override;
    };

}