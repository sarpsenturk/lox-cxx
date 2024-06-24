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

    ExprPtr Parser::parse()
    {
        return expression();
    }

    ExprPtr Parser::expression()
    {
        return equality();
    }

    ExprPtr Parser::equality()
    {
        auto expr = comparison();
        while (consume_expected({{TokenType::Bang, TokenType::BangEqual}})) {
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
            return std::make_unique<LiteralExpr>(string, std::string{string.lexeme});
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
        panic("expected expression");
    }

    bool Parser::is_eof() const
    {
        return current_token_ >= tokens_.size();
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

    bool Parser::consume_expected(TokenType expected)
    {
        if (is_eof() || peek().type != expected) {
            return false;
        }
        consume();
        return true;
    }

    bool Parser::consume_expected(std::span<const TokenType> expected)
    {
        for (auto type : expected) {
            if (consume_expected(type)) {
                return true;
            }
        }
        return false;
    }

    Token Parser::last_token() const
    {
        return tokens_[current_token_ - 1];
    }

    void Parser::panic(const char* msg) const
    {
        throw LoxError(msg, peek().location);
    }
} // namespace lox
