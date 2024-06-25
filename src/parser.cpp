#include "parser.h"

#include "error.h"

#include <stdexcept>
#include <string>

namespace lox
{
    Parser::Parser(std::span<const Token> tokens)
        : tokens_(tokens)
    {
    }

    ParseResult Parser::parse()
    {
        std::vector<StmtPtr> statements;
        std::vector<LoxError> errors;
        while (!is_eof()) {
            try {
                statements.push_back(declaration());
            } catch (LoxError&& err) {
                errors.push_back(std::move(err));
                synchronize();
            }
        }

        if (!errors.empty()) {
            return tl::unexpected(std::move(errors));
        }
        return ParseResult{std::move(statements)};
    }

    StmtPtr Parser::declaration()
    {
        if (consume_expected(TokenType::Var)) {
            return var_decl();
        }
        return statement();
    }

    StmtPtr Parser::var_decl()
    {
        auto identifier = consume_expected(TokenType::Identifier);
        if (!identifier) {
            panic("expected variable name");
        }

        ExprPtr initializer = nullptr;
        if (consume_expected(TokenType::Equal)) {
            initializer = expression();
        }

        if (!consume_expected(TokenType::Semicolon)) {
            panic("expected ';' after variable declaration");
        }
        return std::make_unique<VarDeclStmt>(*identifier, std::move(initializer));
    }

    StmtPtr Parser::statement()
    {
        if (consume_expected(TokenType::Print)) {
            return print_stmt();
        }
        return expr_stmt();
    }

    StmtPtr Parser::print_stmt()
    {
        auto expr = expression();
        if (!consume_expected(TokenType::Semicolon)) {
            panic("expected ';' after expression");
        }
        return std::make_unique<PrintStmt>(std::move(expr));
    }

    StmtPtr Parser::expr_stmt()
    {
        auto expr = expression();
        if (!consume_expected(TokenType::Semicolon)) {
            panic("expected ';' after expression");
        }
        return std::make_unique<ExprStmt>(std::move(expr));
    }

    ExprPtr Parser::expression()
    {
        return assignment();
    }

    ExprPtr Parser::assignment()
    {
        auto expr = equality();
        if (consume_expected(TokenType::Equal)) {
            auto value = assignment();
            if (const auto* var_expr = dynamic_cast<const VarExpr*>(expr.get())) {
                auto name = var_expr->identifier();
                return std::make_unique<AssignmentExpr>(name, std::move(value));
            }
            panic("invalid assignment target");
        }
        return expr;
    }

    ExprPtr Parser::equality()
    {
        auto expr = comparison();
        while (consume_expected({{TokenType::BangEqual, TokenType::EqualEqual}})) {
            auto op = last_token();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, comparison());
        }
        return expr;
    }

    ExprPtr Parser::comparison()
    {
        auto expr = additive();
        while (consume_expected({{TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual}})) {
            auto op = last_token();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, additive());
        }
        return expr;
    }

    ExprPtr Parser::additive()
    {
        auto expr = multiplicative();
        while (consume_expected({{TokenType::Plus, TokenType::Minus}})) {
            auto op = last_token();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, multiplicative());
        }
        return expr;
    }

    ExprPtr Parser::multiplicative()
    {
        auto expr = unary();
        while (consume_expected({{TokenType::Star, TokenType::Slash}})) {
            auto op = last_token();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, unary());
        }
        return expr;
    }

    ExprPtr Parser::unary()
    {
        if (consume_expected({{TokenType::Bang, TokenType::Minus}})) {
            auto op = last_token();
            return std::make_unique<UnaryExpr>(op, unary());
        }
        return primay();
    }

    ExprPtr Parser::primay()
    {
        if (consume_expected(TokenType::Number)) {
            try {
                auto number = last_token();
                double value = std::stod(std::string{number.lexeme});
                return std::make_unique<LiteralExpr>(number, value);
            } catch (const std::invalid_argument& e) {
                panic("could not convert number literal to a number");
            } catch (const std::out_of_range& e) {
                panic("number literal value is out of range");
            }
        }
        if (consume_expected(TokenType::String)) {
            auto string = last_token();
            return std::make_unique<LiteralExpr>(string, std::string{string.lexeme.substr(1, string.lexeme.length() - 2)});
        }
        if (consume_expected({{TokenType::True, TokenType::False}})) {
            auto boolean = last_token();
            return std::make_unique<LiteralExpr>(boolean, boolean.type == TokenType::True);
        }
        if (consume_expected(TokenType::Nil)) {
            auto nil = last_token();
            return std::make_unique<LiteralExpr>(nil, NilLiteral{});
        }
        if (consume_expected(TokenType::LeftParen)) {
            auto expr = expression();
            if (!consume_expected(TokenType::RightParen)) {
                panic("missing closing )");
            }
            return std::make_unique<ParenExpr>(std::move(expr));
        }
        if (auto identifier = consume_expected(TokenType::Identifier)) {
            return std::make_unique<VarExpr>(*identifier);
        }
        panic("expected expression");
    }

    bool Parser::is_eof() const
    {
        return peek().type == TokenType::Eof;
    }

    Token Parser::consume()
    {
        if (is_eof()) {
            return last_token();
        }
        return tokens_[current_token_++];
    }

    Token Parser::peek() const
    {
        return tokens_[current_token_];
    }

    tl::expected<Token, Token> Parser::consume_expected(TokenType expected)
    {
        if (is_eof() || peek().type != expected) {
            return tl::unexpected(peek());
        }
        return consume();
    }

    tl::expected<Token, Token> Parser::consume_expected(std::span<const TokenType> expected)
    {
        for (auto type : expected) {
            if (auto expected = consume_expected(type)) {
                return expected;
            }
        }
        return tl::unexpected(peek());
    }

    Token Parser::last_token() const
    {
        return tokens_[current_token_ - 1];
    }

    void Parser::panic(const char* msg) const
    {
        throw LoxError(msg, peek().location);
    }

    void Parser::synchronize()
    {
        consume();

        while (!is_eof()) {
            if (last_token().type == TokenType::Semicolon) {
                return;
            }

            switch (peek().type) {
                case TokenType::Class:
                case TokenType::Fun:
                case TokenType::Var:
                case TokenType::For:
                case TokenType::If:
                case TokenType::While:
                case TokenType::Print:
                    return;
                default:
                    break;
            }
            consume();
        }
    }
} // namespace lox
