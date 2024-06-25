#include "tree_walk.h"

#include "error.h"
#include "lox_boolean.h"
#include "lox_nil.h"
#include "lox_number.h"
#include "lox_string.h"

#include <fmt/format.h>

#include <cassert>

namespace lox
{
    namespace
    {
        [[noreturn]] void unsupported_unary_op(const char* op_name, const char* lhs_typename, SourceLocation location)
        {
            throw LoxError(fmt::format("Unsupported unary operation {} with type '{}'", op_name, lhs_typename), location);
        }

        [[noreturn]] void throw_unsupported_binary_op(const char* op_name, const char* lhs_typename, const char* rhs_typename, SourceLocation location)
        {
            throw LoxError(fmt::format("Unsupported binary operation {} with types '{}' & '{}'", op_name, lhs_typename, rhs_typename), location);
        }

        const char* binary_op_name(TokenType binary_op)
        {
            switch (binary_op) {
                case TokenType::Plus:
                    return "add '+'";
                case TokenType::Minus:
                    return "subtract '-'";
                case TokenType::Star:
                    return "multiply '*'";
                case TokenType::Slash:
                    return "divide '/'";
                case TokenType::Greater:
                    return "greater '>'";
                case TokenType::GreaterEqual:
                    return "greater equal '>='";
                case TokenType::Less:
                    return "less '<'";
                case TokenType::LessEqual:
                    return "less equal '<='";
                case TokenType::EqualEqual:
                    return "equal '=='";
                case TokenType::BangEqual:
                    return "not equal '!='";
                default:
                    assert(false && "binary op not handled in binary_op_name()");
            }
        }

        const char* unary_op_name(TokenType unary_op)
        {
            switch (unary_op) {
                case TokenType::Minus:
                    return "negate '-'";
                case TokenType::Bang:
                    return "logical not '!'";
                default:
                    assert(false && "unary op not handled in unary_op_name()");
            }
        }
    } // namespace

    std::shared_ptr<LoxObject> TreeWalkInterpreter::evaluate(const Expr& expr)
    {
        expr.accept(*this);
        return expr_result_;
    }

    void TreeWalkInterpreter::execute(const Stmt& stmt)
    {
        stmt.accept(*this);
    }

    void TreeWalkInterpreter::execute(std::span<const StmtPtr> statements)
    {
        for (const auto& stmt : statements) {
            execute(*stmt);
        }
    }

    void TreeWalkInterpreter::visit(const BinaryExpr& expr)
    {
        auto lhs = evaluate(expr.lhs());
        auto rhs = evaluate(expr.rhs());
        switch (expr.op().type) {
            case TokenType::Plus:
                expr_result_ = lhs->add(rhs.get());
                break;
            case TokenType::Minus:
                expr_result_ = lhs->subtract(rhs.get());
                break;
            case TokenType::Star:
                expr_result_ = lhs->multiply(rhs.get());
                break;
            case TokenType::Slash:
                expr_result_ = lhs->divide(rhs.get());
                break;
            case TokenType::Greater:
                if (auto result = lhs->cmp_greater(rhs.get())) {
                    expr_result_ = std::make_shared<LoxBoolean>(expr.op(), *result);
                }
                break;
            case TokenType::GreaterEqual:
                if (auto result = lhs->cmp_greater_equal(rhs.get())) {
                    expr_result_ = std::make_shared<LoxBoolean>(expr.op(), *result);
                }
                break;
            case TokenType::Less:
                if (auto result = lhs->cmp_less(rhs.get())) {
                    expr_result_ = std::make_shared<LoxBoolean>(expr.op(), *result);
                }
                break;
            case TokenType::LessEqual:
                if (auto result = lhs->cmp_less_equal(rhs.get())) {
                    expr_result_ = std::make_shared<LoxBoolean>(expr.op(), *result);
                }
                break;
            case TokenType::BangEqual:
                if (auto result = lhs->cmp_equal(rhs.get())) {
                    expr_result_ = std::make_shared<LoxBoolean>(expr.op(), !(*result));
                }
                break;
            case TokenType::EqualEqual:
                if (auto result = lhs->cmp_equal(rhs.get())) {
                    expr_result_ = std::make_shared<LoxBoolean>(expr.op(), *result);
                }
                break;
            default:
                assert(false && "unhanled/invalid unary operator");
        }

        if (expr_result_ == nullptr) {
            throw_unsupported_binary_op(binary_op_name(expr.op().type), lhs->type_name(), rhs->type_name(), expr.op().location);
        }
    }

    void TreeWalkInterpreter::visit(const UnaryExpr& expr)
    {
        auto value = evaluate(expr.expr());
        switch (expr.op().type) {
            case TokenType::Minus:
                expr_result_ = value->negate();
                break;
            case TokenType::Bang:
                expr_result_ = std::make_shared<LoxBoolean>(expr.op(), !value->is_truthy());
                break;
            default:
                assert(false && "unhandled/invalid unary operator");
        }

        if (expr_result_ == nullptr) {
            unsupported_unary_op(unary_op_name(expr.op().type), value->type_name(), expr.op().location);
        }
    }

    void TreeWalkInterpreter::visit(const ParenExpr& expr)
    {
        expr.expr().accept(*this);
    }

    void TreeWalkInterpreter::visit(const LiteralExpr& expr)
    {
        auto literal_visitor = [token = expr.literal_token()](auto&& literal) -> std::shared_ptr<LoxObject> {
            using T = std::remove_cvref_t<decltype(literal)>;
            if constexpr (std::is_same_v<T, NilLiteral>) {
                return std::make_shared<LoxNil>(token);
            } else if constexpr (std::is_same_v<T, NumberLiteral>) {
                return std::make_shared<LoxNumber>(token, literal);
            } else if constexpr (std::is_same_v<T, StringLiteral>) {
                return std::make_shared<LoxString>(token, literal);
            } else if constexpr (std::is_same_v<T, BooleanLiteral>) {
                return std::make_shared<LoxBoolean>(token, literal);
            }
        };
        expr_result_ = std::visit(literal_visitor, expr.literal_value());
    }

    void TreeWalkInterpreter::visit(const VarExpr& expr)
    {
        if (auto value = environment_.get(expr.identifier().lexeme)) {
            expr_result_ = std::move(value);
        } else {
            throw LoxError(fmt::format("undefined variable '{}'", expr.identifier().lexeme), expr.identifier().location);
        }
    }

    void TreeWalkInterpreter::visit(const AssignmentExpr& expr)
    {
        auto value = evaluate(expr.value());
        if (!environment_.assign(expr.identifier().lexeme, std::move(value))) {
            throw LoxError(fmt::format("assigning to undefined variable '{}'", expr.identifier().lexeme), expr.identifier().location);
        }
    }

    void TreeWalkInterpreter::visit(const LogicExpr& expr)
    {
        auto lhs = evaluate(expr.lhs());
        if (expr.op().type == TokenType::Or) {
            if (lhs->is_truthy()) {
                expr_result_ = std::move(lhs);
            } else {
                expr_result_ = evaluate(expr.rhs());
            }
        } else if (expr.op().type == TokenType::And) {
            if (!lhs->is_truthy()) {
                expr_result_ = std::move(lhs);
            } else {
                expr_result_ = evaluate(expr.rhs());
            }
        } else {
            assert(false && "invalid/unhandled logical operator");
        }
    }

    void TreeWalkInterpreter::visit(const ExprStmt& stmt)
    {
        evaluate(stmt.expr());
        expr_result_ = nullptr;
    }

    void TreeWalkInterpreter::visit(const PrintStmt& stmt)
    {
        auto value = evaluate(stmt.expr());
        fmt::println("{}", value->to_string());
    }

    void TreeWalkInterpreter::visit(const VarDeclStmt& stmt)
    {
        auto value = stmt.initializer() ? evaluate(*stmt.initializer()) : std::make_shared<LoxNil>(stmt.identifier());
        environment_.define(stmt.identifier().lexeme, std::move(value));
    }

    void TreeWalkInterpreter::visit(const BlockStmt& stmt)
    {
        Environment enclosing = std::move(environment_);
        environment_ = Environment{&enclosing};

        // TODO: This try catch is very ugly.
        //  Figure out a better way to do this with RAII
        try {
            for (const auto& statement : stmt.statements()) {
                execute(*statement);
            }
        } catch (...) {
            environment_ = std::move(enclosing);
            throw;
        }
        environment_ = std::move(enclosing);
    }

    void TreeWalkInterpreter::visit(const IfStmt& stmt)
    {
        if (evaluate(stmt.condition())->is_truthy()) {
            execute(stmt.then_branch());
        } else if (stmt.else_branch() != nullptr) {
            execute(*stmt.else_branch());
        }
    }

    void TreeWalkInterpreter::visit(const WhileStmt& stmt)
    {
        while (evaluate(stmt.condition())->is_truthy()) {
            execute(stmt.body());
        }
    }
} // namespace lox
