#include "raw/RawReader.hpp"
#include "implementation/misc/Logger.hpp"
#include "Helper.hpp"

namespace orbtool
{

    bool RawReader::ReadDirective()
    {
        if (MatchLiteral("read"))
            Read_ReadDirective();
        else if (MatchLiteral("new"))
            Read_NewDirective();
        else 
        {
            Expect(TokenType::TOKEN_EOF);
            return false;
        }

        return true;
    }

    void RawReader::BeginBlock()
    {
        Expect(TokenType::TOKEN_LCURLY);
    }

    void RawReader::EndBlock()
    {
        Expect(TokenType::TOKEN_RCURLY);
    }

    void RawReader::Read_ReadDirective() 
    {
        BeginBlock();
        while(Match(TokenType::TOKEN_STRING))
        {
            auto file = PreviousToken().lexeme;
            if (MatchLiteral("as"))
            {
                if (MatchLiteral("TEXTURE"))
                    Read_Texture(file);
                else if (MatchLiteral("VERTEX_SHADER_CODE"))
                    Read_ShaderCode(file, ShaderType::SHADER_VERTEX);
                else if (MatchLiteral("PIXEL_SHADER_CODE"))
                    Read_ShaderCode(file, ShaderType::SHADER_PIXEL);
                else if (MatchLiteral("GEOMETRY_SHADER_CODE"))
                    Read_ShaderCode(file, ShaderType::SHADER_GEOMETRY);
                else if (MatchLiteral("DOMAIN_SHADER_CODE"))
                    Read_ShaderCode(file, ShaderType::SHADER_DOMAIN);
                else if (MatchLiteral("COMPUTE_SHADER_CODE"))
                    Read_ShaderCode(file, ShaderType::SHADER_COMPUTE);
                else if (MatchLiteral("HULL_SHADER_CODE"))
                    Read_ShaderCode(file, ShaderType::SHADER_HULL);
                else if (MatchLiteral("VERTEX_SHADER"))
                    Read_Shader(file, ShaderType::SHADER_VERTEX);
                else if (MatchLiteral("PIXEL_SHADER"))
                    Read_Shader(file, ShaderType::SHADER_PIXEL);
                else if (MatchLiteral("GEOMETRY_SHADER"))
                    Read_Shader(file, ShaderType::SHADER_GEOMETRY);
                else if (MatchLiteral("DOMAIN_SHADER"))
                    Read_Shader(file, ShaderType::SHADER_DOMAIN);
                else if (MatchLiteral("COMPUTE_SHADER"))
                    Read_Shader(file, ShaderType::SHADER_COMPUTE);
                else if (MatchLiteral("HULL_SHADER"))
                    Read_Shader(file, ShaderType::SHADER_HULL);
                else
                {
                    Error("Expected Resource identifier got '%s'", TokenTypeToString(CurrentToken().type));
                }
            }
            else
            {
                Do_ReadFile(file, m_orb, MatchLiteral("triangulate"));
            }

            Expect(TokenType::TOKEN_SEMICOLON);
        }
        EndBlock();
    }

    void RawReader::Read_Texture(const fs::path& path)
    {
        OrbTexture texture;
        texture.texturePath = path;
        Expect(TokenType::TOKEN_LPAREN);
        auto name = Expect(TokenType::TOKEN_STRING).lexeme;
        Expect(TokenType::TOKEN_RPAREN);
        if (MatchLiteral("reference"))
        {
            texture.onlyReference = true;
            Expect(TokenType::TOKEN_LPAREN);
            texture.referencePath = Expect(TokenType::TOKEN_STRING).lexeme;
            Expect(TokenType::TOKEN_RPAREN);
        }
        
        m_orb->AppendObject(name, texture);
    }   

    void RawReader::Read_ShaderCode(const fs::path& path, ShaderType shader_type)
    {
        OrbShaderCode code;
        code.type = shader_type;
        code.shader_code = path;
        Expect(TokenType::TOKEN_LPAREN);
        auto name = Expect(TokenType::TOKEN_STRING).lexeme;
        Expect(TokenType::TOKEN_COMMA);
        code.shader_model = Expect(TokenType::TOKEN_STRING).lexeme;
        if (code.shader_model != "glsl")
        {
            Expect(TokenType::TOKEN_COMMA);
            code.entry_point = Expect(TokenType::TOKEN_STRING).lexeme;
        }
        Expect(TokenType::TOKEN_RPAREN);
        if (MatchLiteral("reference"))
        {
            code.type = orbit::GetReferenceType(shader_type);
            Expect(TokenType::TOKEN_LPAREN);
            code.reference_path = Expect(TokenType::TOKEN_STRING).lexeme;
            Expect(TokenType::TOKEN_RPAREN);
        }
        else if (MatchLiteral("compile"))
        {
            code.compile = true;
            code.type = orbit::GetBinaryType(shader_type);
            if (Match(TokenType::TOKEN_LPAREN))
            {
                do {
                    code.compiler_macros.emplace_back(Expect(TokenType::TOKEN_STRING).lexeme);
                } while (Match(TokenType::TOKEN_COMMA));
                Expect(TokenType::TOKEN_RPAREN);
            }
        }
        m_orb->AppendObject(name, code);
    }

    void RawReader::Read_Shader(const fs::path& file, ShaderType shader_type)
    {
        OrbShaderBinary code;
        code.type = orbit::GetBinaryType(shader_type);
        code.shader_binary = file;
        Expect(TokenType::TOKEN_LPAREN);
        auto name = Expect(TokenType::TOKEN_STRING).lexeme;
        Expect(TokenType::TOKEN_RPAREN);
        if (MatchLiteral("reference"))
        {
            code.type = orbit::GetReferenceType(shader_type);
            Expect(TokenType::TOKEN_LPAREN);
            code.reference_path = Expect(TokenType::TOKEN_STRING).lexeme;
            Expect(TokenType::TOKEN_RPAREN);
        }
        m_orb->AppendObject(name, code);
    }

    void RawReader::Read_NewDirective()
    {
        auto resourceType = Expect(TokenType::TOKEN_LITERAL).lexeme;
        ExpectLiteral("as");
        auto name = Expect(TokenType::TOKEN_STRING).lexeme;
        Expect(TokenType::TOKEN_LCURLY);
        if (resourceType == "INPUT_LAYOUT") 
            m_orb->AppendObject(name, Read_InputLayout());
        else if (resourceType == "PIPELINE_STATE")
            m_orb->AppendObject(name, Read_PipelineState());
        else if (resourceType == "RASTERIZER_STATE")
            m_orb->AppendObject(name, Read_RasterizerState());
        else if (resourceType == "MATERIAL")
            m_orb->AppendObject(name, Read_Material());
        else if (resourceType == "BLEND_STATE")
            m_orb->AppendObject(name, Read_BlendState());
        else if (resourceType == "SAMPLER_STATE")
            m_orb->AppendObject(name, Read_SamplerState());
        else
        {
            Error("Expected Resource identifier got '%s'", resourceType.c_str());
        }
    }

    OrbRasterizerState RawReader::Read_RasterizerState()
    {
        OrbRasterizerState state;
        while(!Match(TokenType::TOKEN_RCURLY))
        {
            auto stateDecl = Expect(TokenType::TOKEN_LITERAL).lexeme;
            ExpectLiteral("as");
            if (stateDecl == "FILLMODE")
            {
                auto fillMode = Expect(TokenType::TOKEN_LITERAL).lexeme;
                if (fillMode == "SOLID")
                    state.fill_mode = FillMode::FILL_SOLID;
                else if (fillMode == "WIREFRAME")
                    state.fill_mode = FillMode::FILL_WIREFRAME;
                else Error("Unknown fill mode: '%s'", fillMode.c_str());
            }
            else if (stateDecl == "CULLMODE")
            {
                auto cullMode = Expect(TokenType::TOKEN_LITERAL).lexeme;
                if (cullMode == "NONE")
                    state.cull_mode = CullMode::CULL_NONE;
                else if (cullMode == "BACK")
                    state.cull_mode = CullMode::CULL_BACK;
                else if (cullMode == "FRONT")
                    state.cull_mode = CullMode::CULL_FRONT;
                else Error("Unknown cull mode: '%s'", cullMode.c_str());
            }
            else Error("Unknown rasterizer state declaration: '%s'", stateDecl.c_str());

            Expect(TokenType::TOKEN_SEMICOLON);
        }
        return state;
    }

    OrbInputLayout RawReader::Read_InputLayout()
    {
        OrbInputLayout layout;
        if (!Match(TokenType::TOKEN_RCURLY))
        {
            do {
                Expect(TokenType::TOKEN_LCURLY);
                OrbInputLayoutElement element;
                element.semantic_name = Expect(TokenType::TOKEN_STRING).lexeme;
                Expect(TokenType::TOKEN_COMMA);
                auto format = Expect(TokenType::TOKEN_LITERAL).lexeme;
                if (format == "FLOAT1")
                    element.format = FormatType::FORMAT_FLOAT1;
                else if (format == "FLOAT2")
                    element.format = FormatType::FORMAT_FLOAT2;
                else if (format == "FLOAT3")
                    element.format = FormatType::FORMAT_FLOAT3;
                else if (format == "FLOAT4")
                    element.format = FormatType::FORMAT_FLOAT4;
                else
                    Error("Unknown format type '%s'", format.c_str());
                
                if (Match(TokenType::TOKEN_COMMA))
                {
                    auto type = Expect(TokenType::TOKEN_LITERAL).lexeme;
                    if (type == "VERTEX_DATA")
                        element.type = DataType::VERTEX_DATA;
                    else if (type == "INSTANCE_DATA")
                        element.type = DataType::INSTANCE_DATA;
                    else
                        Error("Unknown data usage type '%s'", type.c_str());
                    
                    if (Match(TokenType::TOKEN_COMMA))
                    {
                        element.semantic_index = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);
                        if (Match(TokenType::TOKEN_COMMA))
                            element.inputSlot = strtoul(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr, 10);
                    }
                }
                Expect(TokenType::TOKEN_RCURLY);
                layout.elements.emplace_back(std::move(element));
            } while (Match(TokenType::TOKEN_COMMA));
        }
        Expect(TokenType::TOKEN_RCURLY);
        return layout;
    }

    OrbPipelineState RawReader::Read_PipelineState()
    {
        OrbPipelineState state;
        while(!Match(TokenType::TOKEN_RCURLY))
        {
            auto objectType = Expect(TokenType::TOKEN_LITERAL).lexeme;
            ExpectLiteral("as");
            if (MatchLiteral("new"))
            {
                auto name = Expect(TokenType::TOKEN_STRING).lexeme;
                Expect(TokenType::TOKEN_LCURLY);
                if (objectType == "INPUT_LAYOUT")
                {
                    m_orb->AppendObject(name, Read_InputLayout());
                    state.iLayoutId = name;
                }
                else if (objectType == "RASTERIZER_STATE")
                {
                    m_orb->AppendObject(name, Read_RasterizerState());
                    state.rsStateId = name;
                }
                else if (objectType == "BLEND_STATE")
                {
                    m_orb->AppendObject(name, Read_BlendState());
                    state.bsStateId = name;
                }
                else if (objectType == "SAMPLER_STATE")
                {
                    auto sState = Read_SamplerState();
                    ExpectLiteral("with");
                    ExpectLiteral("SLOT_INDEX");
                    auto slotIndex = std::stoi(Expect(TokenType::TOKEN_NUMBER).lexeme);

                    state.sStateIds.emplace(slotIndex, name);
                    m_orb->AppendObject(name, sState);
                }
                else
                    Error("Invalid object type '%s'", name.c_str());
            }
            else
            {
                if (objectType == "VERTEX_SHADER")
                    state.vShaderId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "PIXEL_SHADER")
                    state.pShaderId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "GEOMETRY_SHADER")
                    state.gShaderId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "HULL_SHADER")
                    state.hShaderId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "DOMAIN_SHADER")
                    state.dShaderId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "INPUT_LAYOUT")
                    state.iLayoutId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "RASTERIZER_STATE")
                    state.rsStateId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "BLEND_STATE")
                    state.bsStateId = Expect(TokenType::TOKEN_STRING).lexeme;
                else if (objectType == "SAMPLER_STATE")
                {
                    Expect(TokenType::TOKEN_LPAREN);
                    auto stateId = Expect(TokenType::TOKEN_STRING).lexeme;
                    Expect(TokenType::TOKEN_COMMA);
                    auto samplerStateIndex = std::stoi(Expect(TokenType::TOKEN_NUMBER).lexeme);
                    Expect(TokenType::TOKEN_RPAREN);
                    state.sStateIds.emplace(samplerStateIndex, stateId);
                }
                else if (objectType == "PRIMITIVE_TYPE")
                {
                    auto type = Expect(TokenType::TOKEN_LITERAL).lexeme;
                    if (type == "TRIANGLES")
                        state.primitiveType = EPrimitiveType::TRIANGLES;
                    else if (type == "TRIANGLE_STRIP")
                        state.primitiveType = EPrimitiveType::TRIANGLE_STRIP;
                    else if (type == "TRIANGLE_FAN")
                        state.primitiveType = EPrimitiveType::TRIANGLE_FAN;
                    else if (type == "LINES")
                        state.primitiveType = EPrimitiveType::LINES;
                    else if (type == "POINTS")
                        state.primitiveType = EPrimitiveType::POINTS;
                    else 
                        Error("Invalid primitive type: %s", type.c_str());
                }
                else
                    Error("Unknown pipeline state type '%s'", objectType.c_str());
            }
            Expect(TokenType::TOKEN_SEMICOLON);
        }
        return state;
    }

    OrbBlendState RawReader::Read_BlendState()
    {
        OrbBlendState state;
        state.channelMask = static_cast<int8_t>(EChannel::CHANNEL_ALL);

        auto read_blend_operation = [&]() -> EBlendOp {
            auto op = Expect(TokenType::TOKEN_LITERAL).lexeme;
            if (op == "ADD")
                return EBlendOp::BLEND_ADD;
            else if (op == "SUBTRACT")
                return EBlendOp::BLEND_SUBTRACT;
            else if (op == "REV_SUBTRACT")
                return EBlendOp::BLEND_RSUBTRACT;
            else if (op == "MIN")
                return EBlendOp::BLEND_MIN;
            else if (op == "MAX")
                return EBlendOp::BLEND_MAX;
            else
            {
                Error("Unknown blend operation: '%s'.", op.c_str());
                return EBlendOp::BLEND_ADD;
            }
        };
        auto read_blend = [&]() -> EBlend {
            auto b = Expect(TokenType::TOKEN_LITERAL).lexeme;
            if (b == "ZERO")
                return EBlend::BLEND_ZERO;
            else if (b == "ONE")
                return EBlend::BLEND_ONE;
            else if (b == "SRC_ALPHA")
                return EBlend::BLEND_SRC_ALPHA;
            else if (b == "INV_SRC_ALPHA")
                return EBlend::BLEND_INV_SRC_ALPHA;
            else if (b == "SRC_COLOR")
                return EBlend::BLEND_SRC_COLOR;
            else if (b == "INV_SRC_COLOR")
                return EBlend::BLEND_INV_SRC_COLOR;
            else if (b == "DEST_ALPHA")
                return EBlend::BLEND_DEST_ALPHA;
            else if (b == "INV_DEST_ALPHA")
                return EBlend::BLEND_INV_DEST_ALPHA;
            else if (b == "DEST_COLOR")
                return EBlend::BLEND_DEST_COLOR;
            else if (b == "INV_DEST_COLOR")
                return EBlend::BLEND_INV_DEST_COLOR;
            else if (b == "SRC_ALPHA_SAT")
                return EBlend::BLEND_SRC_ALPHA_SAT;
            else if (b == "BLEND_FACTOR")
                return EBlend::BLEND_BLEND_FACTOR;
            else if (b == "INV_BLEND_FACTOR")
                return EBlend::BLEND_INV_BLEND_FACTOR;
            else 
            {
                Error("Expected blend source, got '%s'.", b.c_str());
                return EBlend::BLEND_SRC_ALPHA;
            }            
        };

        while(!Match(TokenType::TOKEN_RCURLY))
        {
            auto identifier = Expect(TokenType::TOKEN_LITERAL).lexeme;
            ExpectLiteral("as");

            if (identifier == "BLEND_ENABLE")
                state.blendEnabled = ExpectBoolean();
            else if (identifier == "ALPHA_TO_COVERAGE")
                state.alphaToCoverageEnabled = ExpectBoolean();
            else if (identifier == "BLEND_OP")
                state.blendOperation = read_blend_operation();
            else if (identifier == "ALPHA_BLEND_OP")
                state.alphaBlendOperation = read_blend_operation();
            else if (identifier == "SRC_BLEND")
                state.srcBlend = read_blend();
            else if (identifier == "SRC_BLEND_ALPHA")
                state.srcAlphaBlend = read_blend();
            else if (identifier == "DEST_BLEND")
                state.destBlend = read_blend();
            else if (identifier == "DEST_BLEND_ALPHA")
                state.destAlphaBlend = read_blend();
            else if (identifier == "CHANNEL_MASK")
            {
                int8_t mask = 0;
                do {
                    auto m = Expect(TokenType::TOKEN_LITERAL).lexeme;
                    if (m == "ALL")
                        mask |= static_cast<int8_t>(EChannel::CHANNEL_ALL);
                    else if (m == "RED")
                        mask |= static_cast<int8_t>(EChannel::CHANNEL_RED);
                    else if (m == "GREEN")
                        mask |= static_cast<int8_t>(EChannel::CHANNEL_GREEN);
                    else if (m == "BLUE")
                        mask |= static_cast<int8_t>(EChannel::CHANNEL_BLUE);
                    else if (m == "ALPHA")
                        mask |= static_cast<int8_t>(EChannel::CHANNEL_ALPHA);
                    else
                        Error("Invalid channel name: '%s'.", m.c_str());
                } while(MatchLiteral("or"));
                state.channelMask = mask;
            }
            else
            {
                Error("Unknown blend state property: '%s'.", identifier.c_str());
            }
            Expect(TokenType::TOKEN_SEMICOLON);
        }
        return state;
    }

    OrbSamplerState RawReader::Read_SamplerState()
    {
        OrbSamplerState state;
        auto read_filter = [&]() -> EFilter {
            auto filter = Expect(TokenType::TOKEN_LITERAL).lexeme;
            if (filter == "MIN_MAG_MIP_POINT")
                return EFilter::MIN_MAG_MIP_POINT;
            else if (filter == "MIN_MAG_POINT_MIP_LINEAR")
                return EFilter::MIN_MAG_POINT_MIP_LINEAR;
            else if (filter == "MIN_POINT_MAG_LINEAR_MIP_POINT")
                return EFilter::MIN_POINT_MAG_LINEAR_MIP_POINT;
            else if (filter == "MIN_POINT_MAG_MIP_LINEAR")
                return EFilter::MIN_POINT_MAG_MIP_LINEAR;
            else if (filter == "MIN_LINEAR_MAG_MIP_POINT")
                return EFilter::MIN_LINEAR_MAG_MIP_POINT;
            else if (filter == "MIN_LINEAR_MAG_POINT_MIP_LINEAR")
                return EFilter::MIN_LINEAR_MAG_POINT_MIP_LINEAR;
            else if (filter == "MIN_MAG_LINEAR_MIP_POINT")
                return EFilter::MIN_MAG_LINEAR_MIP_POINT;
            else if (filter == "MIN_MAG_MIP_LINEAR")
                return EFilter::MIN_MAG_MIP_LINEAR;
            else if (filter == "ANISOTROPIC")
                return EFilter::ANISOTROPIC;

            Error("Invalid filter: %s\n", filter.c_str());
            return EFilter::MIN_MAG_MIP_POINT;
        };
        auto read_address = [&]() -> EAddress {
            auto address = Expect(TokenType::TOKEN_LITERAL).lexeme;
            if (address == "WRAP")
                return EAddress::WRAP;
            else if (address == "CLAMP")
                return EAddress::CLAMP;
            else if (address == "BORDER")
                return EAddress::BORDER;
            else if (address == "MIRROR")
                return EAddress::MIRROR;
            
            Error("Invalid address mode: %s\n", address.c_str());
            return EAddress::WRAP;
        };
        while(!Match(TokenType::TOKEN_RCURLY))
        {
            auto property = Expect(TokenType::TOKEN_LITERAL).lexeme;
            ExpectLiteral("as");
            if (property == "FILTER")
                state.filter = read_filter();
            else if (property == "ADDRESS_X1")
                state.addressX1 = read_address();
            else if (property == "ADDRESS_X2")
                state.addressX2 = read_address();
            else if (property == "ADDRESS_X3")
                state.addressX3 = read_address();                        

            Expect(TokenType::TOKEN_SEMICOLON);
        }

        return state;
    }

    OrbMaterial RawReader::Read_Material()
    {
        OrbMaterial material;
        while(!Match(TokenType::TOKEN_RCURLY))
        {
            auto identifier = Expect(TokenType::TOKEN_LITERAL).lexeme;
            ExpectLiteral("as");
            auto read_color = [&]() {
                Vector4f color;
                Expect(TokenType::TOKEN_LPAREN);
                color.x() = strtod(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                Expect(TokenType::TOKEN_COMMA);
                color.y() = strtod(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                Expect(TokenType::TOKEN_COMMA);
                color.z() = strtod(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                Expect(TokenType::TOKEN_COMMA);
                color.w() = strtod(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
                Expect(TokenType::TOKEN_RPAREN);
                return color;
            };

            if (identifier == "DIFFUSE_COLOR")
                material.diffuse = read_color();
            else if (identifier == "SPECULAR_COLOR")
                material.specular = read_color();
            else if (identifier == "ROUGHNESS")
                material.roughness = strtod(Expect(TokenType::TOKEN_NUMBER).lexeme.c_str(), nullptr);
            else if (identifier == "DIFFUSE_TEXTURE")
                material.diffuseTextureId = Expect(TokenType::TOKEN_STRING).lexeme;
            else if (identifier == "NORMAL_MAP")
                material.normalMapId = Expect(TokenType::TOKEN_STRING).lexeme;
            else if (identifier == "ROUGHNESS_MAP")
                material.roughnessMapId = Expect(TokenType::TOKEN_STRING).lexeme;
            else if (identifier == "OCCLUSION_MAP")
                material.occlusionMapId = Expect(TokenType::TOKEN_STRING).lexeme;
            else 
                Error("Unknown material property '%s'", identifier.c_str());

            Expect(TokenType::TOKEN_SEMICOLON);
        }
        return material;
    }

    bool RawReader::ReadFile(const fs::path& filepath, OrbIntermediate* orb)
    {
        if (!OpenFile(filepath))
            return false;
        
        m_orb = orb;
        Advance();

        while(!EndOfFile())
        {
            ReadDirective();
            Expect(TokenType::TOKEN_SEMICOLON);
        }
        
        return true;
    }

}