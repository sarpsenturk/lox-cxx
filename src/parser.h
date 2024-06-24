#pragma once

#include "ast.h"
#include "token.h"

#include <span>

namespace lox
{
    class Parser
    {
    public:
        explicit Parser(std::span<const Token> tokens);

        ExprPtr parse();

    private:
        ExprPtr expression();
        ExprPtr equality();
        ExprPtr comparison();
        ExprPtr additive();
        ExprPtr multiplicative();
        ExprPtr unary();
        ExprPtr primay();

        [[nodiscar]] bool is_eof() const;

        Token consume();
        Token peek() const;
        bool consume_expected(TokenType expected);
        bool consume_expected(std::span<const TokenType> expected);

        Token last_token() const;

        [[noreturn]] void panic(const char* msg) const;

        std::span<const Token> tokens_;
        std::size_t current_token_ = 0;
    };
} // namespace lox
