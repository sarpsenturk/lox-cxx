#pragma once

#include "ast.h"
#include "error.h"
#include "token.h"

#include <tl/expected.hpp>

#include <span>
#include <vector>

namespace lox
{
    using ParseResult = tl::expected<std::vector<StmtPtr>, std::vector<LoxError>>;

    class Parser
    {
    public:
        explicit Parser(std::span<const Token> tokens);

        ParseResult parse();

    private:
        StmtPtr declaration();
        StmtPtr var_decl();
        StmtPtr statement();
        StmtPtr print_stmt();
        StmtPtr block_stmt();
        StmtPtr expr_stmt();
        ExprPtr expression();
        ExprPtr assignment();
        ExprPtr equality();
        ExprPtr comparison();
        ExprPtr additive();
        ExprPtr multiplicative();
        ExprPtr unary();
        ExprPtr primay();

        [[nodiscar]] bool is_eof() const;

        Token consume();
        Token peek() const;
        tl::expected<Token, Token> consume_expected(TokenType expected);
        tl::expected<Token, Token> consume_expected(std::span<const TokenType> expected);

        Token last_token() const;

        [[noreturn]] void panic(const char* msg) const;
        void synchronize();

        std::span<const Token> tokens_;
        std::size_t current_token_ = 0;
    };
} // namespace lox
