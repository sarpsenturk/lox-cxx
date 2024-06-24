#pragma once

#include "token.h"

#include <memory>
#include <string>
#include <variant>

namespace lox
{
    class BinaryExpr;
    class UnaryExpr;
    class ParenExpr;
    class LiteralExpr;

    class ExprVisitor
    {
    public:
        ExprVisitor() = default;
        virtual ~ExprVisitor() = default;

        virtual void visit(const BinaryExpr& expr) = 0;
        virtual void visit(const UnaryExpr& expr) = 0;
        virtual void visit(const ParenExpr& expr) = 0;
        virtual void visit(const LiteralExpr& expr) = 0;

    protected:
        ExprVisitor(const ExprVisitor&) = default;
        ExprVisitor(ExprVisitor&&) = default;
        ExprVisitor& operator=(const ExprVisitor&) = default;
        ExprVisitor& operator=(ExprVisitor&&) = default;
    };

    class Expr
    {
    public:
        Expr() = default;
        virtual ~Expr() = default;

        virtual void accept(ExprVisitor& visitor) const = 0;

    protected:
        Expr(const Expr&) = default;
        Expr(Expr&&) = default;
        Expr& operator=(const Expr&) = default;
        Expr& operator=(Expr&&) = default;
    };

    using ExprPtr = std::unique_ptr<Expr>;

    class BinaryExpr : public Expr
    {
    public:
        BinaryExpr(ExprPtr lhs, Token op, ExprPtr rhs);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& lhs() const { return *lhs_; }
        [[nodiscard]] auto& op() const { return op_; }
        [[nodiscard]] auto& rhs() const { return *rhs_; }

    private:
        ExprPtr lhs_;
        Token op_;
        ExprPtr rhs_;
    };

    class UnaryExpr : public Expr
    {
    public:
        UnaryExpr(Token op, ExprPtr expr);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& op() const { return op_; }
        [[nodiscard]] auto& expr() const { return *expr_; }

    private:
        Token op_;
        ExprPtr expr_;
    };

    class ParenExpr : public Expr
    {
    public:
        ParenExpr(ExprPtr expr);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& expr() const { return *expr_; }

    private:
        ExprPtr expr_;
    };

    using NilLiteral = std::monostate;
    using NumberLiteral = double;
    using StringLiteral = std::string;
    using BooleanLiteral = bool;
    using LiteralValue = std::variant<NilLiteral, NumberLiteral, StringLiteral, BooleanLiteral>;

    class LiteralExpr : public Expr
    {
    public:
        LiteralExpr(Token literal, LiteralValue value);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& literal_value() const { return value_; }
        [[nodiscard]] NumberLiteral as_number() const { return std::get<NumberLiteral>(value_); }
        [[nodiscard]] StringLiteral as_string() const { return std::get<StringLiteral>(value_); }
        [[nodiscard]] BooleanLiteral as_bool() const { return std::get<BooleanLiteral>(value_); }

        [[nodiscard]] auto& literal_token() const { return literal_; }

    private:
        Token literal_;
        LiteralValue value_;
    };
} // namespace lox
