#pragma once

#include "source_location.h"

#include <string_view>

namespace lox
{
    enum class TokenType {
        Eof = -1,

        Plus,
        Minus,
        Star,
        Slash,
        LeftParen,
        RightParen,
        LeftBrace,
        RightBrace,
        Semicolon,
        Comma,
        Dot,

        Bang,
        BangEqual,
        Equal,
        EqualEqual,
        Greater,
        GreaterEqual,
        Less,
        LessEqual,

        Identifier,
        String,
        Number,

        And,
        Or,

        True,
        False,

        Var,
        Fun,
        Class,

        If,
        Else,
        For,
        While,

        Super,
        Nil,
    };

    struct Token {
        TokenType type;
        std::string_view lexeme;
        SourceLocation location;
    };

    const char* format_as(TokenType type);
} // namespace lox
