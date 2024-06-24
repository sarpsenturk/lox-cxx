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
        virtual void visit(const LiteralExpr& expr);

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

        virtual void accept(ExprVisitor& visitor) = 0;

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

        void accept(ExprVisitor& visitor) override { return visitor.visit(*this); }

    private:
        ExprPtr lhs_;
        Token op_;
        ExprPtr rhs_;
    };

    class UnaryExpr : public Expr
    {
    public:
        UnaryExpr(Token op, ExprPtr expr);

        void accept(ExprVisitor& visitor) override { return visitor.visit(*this); }

    private:
        Token op_;
        ExprPtr expr_;
    };

    class ParenExpr : public Expr
    {
    public:
        ParenExpr(ExprPtr expr);

        void accept(ExprVisitor& visitor) override { return visitor.visit(*this); }

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

        void accept(ExprVisitor& visitor) override { return visitor.visit(*this); }

    private:
        Token literal_;
        LiteralValue value_;
    };
} // namespace lox
