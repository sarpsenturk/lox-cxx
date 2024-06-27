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
            } catch (const LoxError& err) {
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
        if (consume_expected(TokenType::Fun)) {
            return fun_decl();
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

    StmtPtr Parser::fun_decl()
    {
        auto identifier = consume_expected(TokenType::Identifier);
        if (!identifier) {
            panic("expected identifier");
        }

        if (!consume_expected(TokenType::LeftParen)) {
            panic("expected '(' to start parameter list");
        }

        std::vector<Token> parameters;
        if (peek().type != TokenType::RightParen) {
            do {
                if (parameters.size() >= 255) {
                    panic("can't have more than 255 parameters");
                }
                if (auto parameter = consume_expected(TokenType::Identifier)) {
                    parameters.push_back(*parameter);
                } else {
                    panic("expected parameter name");
                }
            } while (consume_expected(TokenType::Comma));
        }
        if (!consume_expected(TokenType::RightParen)) {
            panic("expected closing ')' after parameter list");
        }
        if (!consume_expected(TokenType::LeftBrace)) {
            panic("expected '{' before body");
        }

        return std::make_unique<FunDeclStmt>(*identifier, std::move(parameters), block_stmt());
    }

    StmtPtr Parser::statement()
    {
        if (consume_expected(TokenType::If)) {
            return if_stmt();
        }
        if (consume_expected(TokenType::While)) {
            return while_stmt();
        }
        if (consume_expected(TokenType::For)) {
            return for_stmt();
        }
        if (consume_expected(TokenType::Return)) {
            return return_stmt();
        }
        if (consume_expected(TokenType::Print)) {
            return print_stmt();
        }
        if (consume_expected(TokenType::LeftBrace)) {
            return block_stmt();
        }
        return expr_stmt();
    }

    StmtPtr Parser::if_stmt()
    {
        if (!consume_expected(TokenType::LeftParen)) {
            panic("expected '(' after if");
        }
        auto condition = expression();
        if (!consume_expected(TokenType::RightParen)) {
            panic("expected ')' after if condition");
        }

        auto then_branch = statement();
        StmtPtr else_branch = nullptr;
        if (consume_expected(TokenType::Else)) {
            else_branch = statement();
        }

        return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
    }

    StmtPtr Parser::while_stmt()
    {
        if (!consume_expected(TokenType::LeftParen)) {
            panic("expected '(' after while");
        }
        auto condition = expression();
        if (!consume_expected(TokenType::RightParen)) {
            panic("expected ')' after while condition");
        }
        return std::make_unique<WhileStmt>(std::move(condition), statement());
    }

    StmtPtr Parser::for_stmt()
    {
        if (!consume_expected(TokenType::LeftParen)) {
            panic("expected '(' after for");
        }

        StmtPtr initializer;
        if (consume_expected(TokenType::Semicolon)) {
            initializer = nullptr;
        } else if (consume_expected(TokenType::Var)) {
            initializer = var_decl();
        } else {
            initializer = expr_stmt();
        }

        ExprPtr condition = nullptr;
        if (peek().type != TokenType::Semicolon) {
            condition = expression();
        }
        if (!consume_expected(TokenType::Semicolon)) {
            panic("expected ';' after loop condition");
        }

        ExprPtr increment = nullptr;
        if (peek().type != TokenType::Semicolon) {
            increment = expression();
        }
        if (!consume_expected(TokenType::RightParen)) {
            panic("expected closing ')' after for loop");
        }

        auto body = statement();
        if (increment != nullptr) {
            // Have to do this hack as you can't just do std::vector<StmtPtr>{std::move(body), ...}
            // because std::initializer_list always copies the elements...
            std::vector<StmtPtr> stmts;
            stmts.reserve(2);
            stmts.push_back(std::move(body));
            stmts.push_back(std::make_unique<ExprStmt>(std::move(increment)));
            body = std::make_unique<BlockStmt>(std::move(stmts));
        }

        if (condition == nullptr) {
            condition = std::make_unique<LiteralExpr>(Token{}, true);
        }
        body = std::make_unique<WhileStmt>(std::move(condition), std::move(body));

        if (initializer != nullptr) {
            // Again this hack is necessary.
            // Probably should create a helper function to create BlockStmt or std::vector<StmtPtr> given n statements
            std::vector<StmtPtr> stmts;
            stmts.reserve(2);
            stmts.push_back(std::move(initializer));
            stmts.push_back(std::move(body));
            body = std::make_unique<BlockStmt>(std::move(stmts));
        }

        return body;
    }

    StmtPtr Parser::return_stmt()
    {
        ExprPtr value = nullptr;
        if (peek().type != TokenType::Semicolon) {
            value = expression();
        }
        if (auto ret = consume_expected(TokenType::Semicolon)) {
            return std::make_unique<ReturnStmt>(*ret, std::move(value));
        }
        panic("expected ';' after return statement");
    }

    StmtPtr Parser::print_stmt()
    {
        auto expr = expression();
        if (!consume_expected(TokenType::Semicolon)) {
            panic("expected ';' after expression");
        }
        return std::make_unique<PrintStmt>(std::move(expr));
    }

    std::unique_ptr<BlockStmt> Parser::block_stmt()
    {
        std::vector<StmtPtr> statements;
        while (peek().type != TokenType::RightBrace && !is_eof()) {
            statements.push_back(declaration());
        }
        if (!consume_expected(TokenType::RightBrace)) {
            panic("expected closing '}' after block");
        }
        return std::make_unique<BlockStmt>(std::move(statements));
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
        auto expr = logical_or();
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

    ExprPtr Parser::logical_or()
    {
        auto expr = logical_and();
        while (consume_expected(TokenType::Or)) {
            auto op = last_token();
            expr = std::make_unique<LogicExpr>(std::move(expr), op, logical_and());
        }
        return expr;
    }

    ExprPtr Parser::logical_and()
    {
        auto expr = equality();
        while (consume_expected(TokenType::And)) {
            auto op = last_token();
            expr = std::make_unique<LogicExpr>(std::move(expr), op, equality());
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
        return call();
    }

    ExprPtr Parser::call()
    {
        auto expr = primary();
        while (consume_expected(TokenType::LeftParen)) {
            std::vector<ExprPtr> arguments;
            if (peek().type != TokenType::RightParen) {
                do {
                    if (arguments.size() >= 255) {
                        panic("can't have more than 255 arguments");
                    }
                    arguments.push_back(expression());
                } while (consume_expected(TokenType::Comma));
            }
            if (auto call_end = consume_expected(TokenType::RightParen)) {
                return std::make_unique<CallExpr>(std::move(expr), std::move(arguments), *call_end);
            }
            panic("expected closing ')' after arguments");
        }
        return expr;
    }

    ExprPtr Parser::primary()
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
