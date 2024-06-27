#pragma once

#include "source_location.h"
#include "token.h"

#include <string_view>
#include <unordered_map>
#include <vector>

namespace lox
{
    class Lexer
    {
    public:
        static const std::unordered_map<std::string_view, TokenType> keywords;

        explicit Lexer(std::string_view source);

        Token tokenize_next();
        std::vector<Token> tokenize();

        [[nodiscard]] bool is_eof() const;

    private:
        std::string_view source_;

        char consume();
        char peek() const;
        char peek_next() const;
        bool consume_expected(char expected);

        std::string_view current_lexeme() const;
        Token make_token(TokenType type) const;
        Token make_string_literal();
        Token make_number_literal();
        Token make_keyword_or_identifier();

        [[noreturn]] void throw_error(const char* message) const;

        std::size_t current_position_ = 0;
        std::size_t start_position_ = 0;
        SourceLocation current_location_ = {.line = 1, .column = 1};
        SourceLocation start_location_ = {.line = 1, .column = 1};
    };
} // namespace lox
