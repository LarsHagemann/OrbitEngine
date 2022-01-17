#pragma once
#include "orb/OrbIntermediate.hpp"
#include "implementation/misc/Logger.hpp"

#include <fstream>
#include <filesystem>
#include <string>

namespace orbtool
{

    namespace fs = std::filesystem;

    enum class TokenType
    {
        TOKEN_LCURLY,     // {
        TOKEN_RCURLY,     // }
        TOKEN_LPAREN,     // (
        TOKEN_RPAREN,     // )
        TOKEN_LBRACKET,   // [
        TOKEN_RBRACKET,   // ]
        TOKEN_SLASH,      // /
        TOKEN_COMMA,      // ,
        TOKEN_SEMICOLON,  // ;
        TOKEN_EQUALS,     // =
        TOKEN_NUMBER,     // -100.24E5
        TOKEN_STRING,     // "abcd"
        TOKEN_LITERAL,    // Ks
        TOKEN_EOF,        // EOF
        TOKEN_ERROR,      // error
    };

    struct Token
    {
        TokenType   type;
        std::string lexeme;
    };

    class Reader
    {
    protected:
        std::ifstream m_file;
        fs::path m_filepath;
        std::string m_lexeme;
        char m_previous = 0;
        char m_current = 0;
        uint32_t m_currentLine = 0u;
        Token m_cToken;
        Token m_pToken;
        OrbIntermediate* m_orb;
    protected:
        bool OpenFile(const fs::path& filepath);
        bool EndOfFile() const;
        char CurrentChar() const;
        char NextChar();
        Token NextToken();
        void SkipWhitespace();
        void Advance();
        Token MakeToken(TokenType type) const;
        Token MakeString() const;
        static const char* TokenTypeToString(TokenType type);
        Token Expect(TokenType type);
        Token ExpectLiteral(const std::string& literal);
        bool ExpectBoolean();
        bool MatchLiteral(const std::string& literal);
        bool Match(TokenType type);
        Token CurrentToken() const { return m_cToken; }
        Token PreviousToken() const { return m_pToken; }
        template<typename...Args>
        Token MakeError(const char* format, Args...args)
        {
            char buffer[1000];
            auto count = sprintf_s(buffer, format, args...);
            return { TokenType::TOKEN_ERROR, std::string(buffer, count) };
        }
        template<typename...Args>
        Token Error(const char* format, Args...args)
        {
            char buffer[1000];
            auto count = sprintf_s(buffer, format, args...);
            ORBIT_THROW("Error at %s@%d: %*s", m_filepath.generic_string().c_str(), m_currentLine, count, buffer);
        }
    public:
        virtual bool ReadFile(const fs::path& filepath, OrbIntermediate* intermediate) = 0;
    };

}