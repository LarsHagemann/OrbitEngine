#include "Reader.hpp"
#include "implementation/misc/Logger.hpp"

namespace orbtool
{

    bool Reader::OpenFile(const fs::path& filepath)
    {
        m_currentLine = 1u;
        m_filepath = filepath;
        m_file.open(filepath, std::ios::binary | std::ios::in);
        auto p = fs::absolute(filepath).parent_path();
        ORBIT_LOG("Working directory: %s", p.generic_string().c_str());
        fs::current_path(p);
        return m_file.is_open();
    }

    char Reader::CurrentChar() const
    {
        return m_current;
    }

    char Reader::NextChar()
    {
        m_previous = m_current;
        m_lexeme += m_previous;
        m_file.read((char*)&m_current, sizeof(char));
        return m_previous;
    }

    void Reader::SkipWhitespace()
    {
        while (!EndOfFile() && (std::isspace(CurrentChar()) || CurrentChar() == '\0'))
        {
            if (CurrentChar() == '\n')
                ++m_currentLine;
            NextChar();
        }
    }

    bool Reader::EndOfFile() const
    {
        return m_file.eof();
    }

    Token Reader::MakeToken(TokenType type) const
    {
        if (type == TokenType::TOKEN_EOF)
            return Token{ type, "" };
        return Token{ type, m_lexeme };
    }

    Token Reader::NextToken()
    {
        SkipWhitespace();
        if (EndOfFile())
            return MakeToken(TokenType::TOKEN_EOF);
        
        m_lexeme = "";
        auto c = NextChar();
        switch (c)
        {
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            while(!EndOfFile() && std::isdigit(CurrentChar()))
                NextChar();
            if (CurrentChar() != '.')
            {
                if (CurrentChar() == 'e' || CurrentChar() == 'E')
                    NextChar();
                
                if (CurrentChar() == '-')
                    NextChar();
                
                while(!EndOfFile() && std::isdigit(CurrentChar()))
                    NextChar();
                
                return MakeToken(TokenType::TOKEN_NUMBER);
            }
            NextChar();
        [[fallthough]] 
        case '.':
            while(!EndOfFile() && std::isdigit(CurrentChar()))
                NextChar();
            
            if (CurrentChar() == 'e' || CurrentChar() == 'E')
                NextChar();

            if (CurrentChar() == '-')
                NextChar();
            
            while(!EndOfFile() && std::isdigit(CurrentChar()))
                NextChar();

            return MakeToken(TokenType::TOKEN_NUMBER);
        case '#':
            while(!EndOfFile() && CurrentChar() != '\n')
                NextChar();
            if (!EndOfFile())
                NextChar();
            ++m_currentLine;
            return NextToken();
        case '"':
            NextChar();
            while (!EndOfFile() && CurrentChar() != '"')
            {
                if (CurrentChar() == '\\')
                {
                    NextChar();
                    if (EndOfFile())
                        return MakeError("");
                }
                NextChar();
            }
            NextChar();
            return MakeString();
        case '(': return MakeToken(TokenType::TOKEN_LPAREN);
        case ')': return MakeToken(TokenType::TOKEN_RPAREN);
        case '[': return MakeToken(TokenType::TOKEN_LBRACKET);
        case ']': return MakeToken(TokenType::TOKEN_RBRACKET);
        case '{': return MakeToken(TokenType::TOKEN_LCURLY);
        case '}': return MakeToken(TokenType::TOKEN_RCURLY);
        case '/': return MakeToken(TokenType::TOKEN_SLASH);
        case ';': return MakeToken(TokenType::TOKEN_SEMICOLON);
        case '=': return MakeToken(TokenType::TOKEN_EQUALS);
        case ',': return MakeToken(TokenType::TOKEN_COMMA);
        default:
            if (!std::isalpha(c))
                return MakeError("Invalid character: %c", c);
            
            while(std::isalnum(CurrentChar()) || CurrentChar() == '_' || CurrentChar() == '.')
                NextChar();

            return MakeToken(TokenType::TOKEN_LITERAL);
        }
    }

    Token Reader::MakeString() const
    {
        std::string str;
        for (auto i = 1u; i < m_lexeme.length() - 1; ++i)
        {
            if (m_lexeme[i] == '\\')
            {
                i += 1;
                switch (m_lexeme[i])
                {
                case '"': str += "\""; break;
                case 't': str += "\t"; break;
                case 'n': str += "\n"; break;
                case 'r': str += "\r"; break;
                case 'b': str += "\b"; break;
                case 'v': str += "\v"; break;
                case 'f': str += "\f"; break;
                case '\\': str += "\\"; break;
                }
            }
            else
            {
                str += m_lexeme[i];
            }
        }
        return { TokenType::TOKEN_STRING, str };
    }

    const char* Reader::TokenTypeToString(TokenType type)
    {
        switch(type)
        {
        case TokenType::TOKEN_LPAREN: return "(";
        case TokenType::TOKEN_RPAREN: return ")";
        case TokenType::TOKEN_LCURLY: return "{";
        case TokenType::TOKEN_RCURLY: return "}";
        case TokenType::TOKEN_LBRACKET: return "[";
        case TokenType::TOKEN_RBRACKET: return "]";
        case TokenType::TOKEN_COMMA: return ",";
        case TokenType::TOKEN_SLASH: return "/";
        case TokenType::TOKEN_SEMICOLON: return ";";
        case TokenType::TOKEN_EQUALS: return "=";
        case TokenType::TOKEN_STRING: return "STRING";
        case TokenType::TOKEN_LITERAL: return "LITERAL";
        case TokenType::TOKEN_NUMBER: return "NUMBER";
        case TokenType::TOKEN_EOF: return "EOF";
        case TokenType::TOKEN_ERROR: return "ERROR";
        }

        return "_Unknown_";
    }

    void Reader::Advance()
    {
        m_pToken = m_cToken;
        m_cToken = NextToken();
        if (m_cToken.type == TokenType::TOKEN_ERROR)
            Error("Invalid token: %s", m_cToken.lexeme.c_str());
    }

    bool Reader::MatchLiteral(const std::string& literal)
    {
        if (m_cToken.lexeme == literal)
        {
            Advance();
            return true;
        }

        return false;
    }

    bool Reader::Match(TokenType type)
    {
        if (m_cToken.type == type)
        {
            Advance();
            return true;
        }

        return false;
    }

    Token Reader::Expect(TokenType type)
    {
        if (m_cToken.type != type)
            Error("Expected token type '%s' but got '%s'", TokenTypeToString(type), TokenTypeToString(m_cToken.type));

        Advance();
        return m_pToken;
    }

    Token Reader::ExpectLiteral(const std::string& literal)
    {
        auto token = Expect(TokenType::TOKEN_LITERAL);
        if (token.lexeme != literal)
            Error("Expected literal '%s' but got '%s'", literal.c_str(), token.lexeme.c_str());

        return token;
    }

}