#include "lexer.h"

#include <gtest/gtest.h>

#include <array>

namespace lox
{
    namespace
    {
        TEST(Lexer, SingleCharTokens)
        {
            const char* source = "+-*/(){};,.";
            auto lexer = Lexer{source};
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Plus);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Minus);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Star);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Slash);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::LeftParen);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::RightParen);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::LeftBrace);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::RightBrace);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Semicolon);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Comma);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Dot);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Eof);
        }

        TEST(Lexer, IgnoreWhitespace)
        {
            const char* source = " \r\t\n+";
            auto lexer = Lexer{source};
            const auto token = lexer.tokenize_next();
            EXPECT_EQ(token.type, TokenType::Plus);
            EXPECT_EQ(token.location, (SourceLocation{2, 1}));
        }

        TEST(Lexer, IgnoreCommentsEOF)
        {
            const char* source = "// This is a comment without new line";
            auto lexer = Lexer{source};
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Eof);
        }

        TEST(Lexer, IgnoreCommentNewline)
        {
            const char* source = "// This is a comment with a new line\n+";
            auto lexer = Lexer{source};
            const auto token = lexer.tokenize_next();
            EXPECT_EQ(token.type, TokenType::Plus);
            EXPECT_EQ(token.location, (SourceLocation{2, 1}));
        }

        TEST(Lexer, MultiCharTokens)
        {
            const char* source = "! != = == > >= < <=";
            auto lexer = Lexer{source};
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Bang);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::BangEqual);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Equal);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::EqualEqual);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Greater);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::GreaterEqual);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Less);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::LessEqual);
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Eof);
        }

        TEST(Lexer, EmptyStringLiteral)
        {
            const char* source = "\"\"";
            auto lexer = Lexer{source};
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::String);
        }

        TEST(Lexer, StringLiteral)
        {
            const char* source = "\"This is a string\"";
            auto lexer = Lexer{source};
            const auto token = lexer.tokenize_next();
            EXPECT_EQ(token.type, TokenType::String);
            EXPECT_EQ(token.lexeme, "\"This is a string\"");
        }

        TEST(Lexer, MultilineStringLiteral)
        {
            const char* source = "\"This is a\nmultiline string\"";
            auto lexer = Lexer{source};
            const auto token = lexer.tokenize_next();
            EXPECT_EQ(token.type, TokenType::String);
            EXPECT_EQ(token.lexeme, "\"This is a\nmultiline string\"");
        }

        TEST(Lexer, NumberLiteralEOF)
        {
            const char* source = "0";
            auto lexer = Lexer{source};
            EXPECT_EQ(lexer.tokenize_next().type, TokenType::Number);
        }

        TEST(Lexer, NumberLiteral)
        {
            const char* source = "123456789";
            auto lexer = Lexer{source};
            const auto token = lexer.tokenize_next();
            EXPECT_EQ(token.type, TokenType::Number);
            EXPECT_EQ(token.lexeme, "123456789");
        }

        TEST(Lexer, FloatingNumberLiteral)
        {
            const char* source = "1234.56789";
            auto lexer = Lexer{source};
            const auto token = lexer.tokenize_next();
            EXPECT_EQ(token.type, TokenType::Number);
            EXPECT_EQ(token.lexeme, "1234.56789");
        }

        TEST(Lexer, Keywords)
        {
            for (auto [keyword, token_type] : Lexer::keywords) {
                const auto source = std::string{keyword};
                auto lexer = Lexer{source};
                EXPECT_EQ(lexer.tokenize_next().type, token_type);
            }
        }

        TEST(Lexer, Identifier)
        {
            const auto valid_identifiers = std::array{
                "identifier",
                "identifier42",
                "iden42tifier",
                "iden_tifier",
                "_identifier",
                "identifier_",
            };
            for (auto identifier : valid_identifiers) {
                auto lexer = Lexer{identifier};
                EXPECT_EQ(lexer.tokenize_next().type, TokenType::Identifier);
            }
        }
    } // namespace
} // namespace lox
