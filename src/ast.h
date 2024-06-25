#pragma once

#include "token.h"

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace lox
{
    class BinaryExpr;
    class UnaryExpr;
    class ParenExpr;
    class LiteralExpr;
    class VarExpr;
    class AssignmentExpr;
    class LogicExpr;

    class ExprVisitor
    {
    public:
        ExprVisitor() = default;
        virtual ~ExprVisitor() = default;

        virtual void visit(const BinaryExpr& expr) = 0;
        virtual void visit(const UnaryExpr& expr) = 0;
        virtual void visit(const ParenExpr& expr) = 0;
        virtual void visit(const LiteralExpr& expr) = 0;
        virtual void visit(const VarExpr& expr) = 0;
        virtual void visit(const AssignmentExpr& expr) = 0;
        virtual void visit(const LogicExpr& expr) = 0;

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

    class VarExpr : public Expr
    {
    public:
        explicit VarExpr(Token identifier);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& identifier() const { return identifier_; }

    private:
        Token identifier_;
    };

    class AssignmentExpr : public Expr
    {
    public:
        AssignmentExpr(Token identifier, ExprPtr value);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& identifier() const { return identifier_; }
        [[nodiscard]] auto& value() const { return *value_; }

    private:
        Token identifier_;
        ExprPtr value_;
    };

    class LogicExpr : public Expr
    {
    public:
        LogicExpr(ExprPtr lhs, Token op, ExprPtr rhs);

        void accept(ExprVisitor& visitor) const override { return visitor.visit(*this); }

        [[nodiscard]] auto& lhs() const { return *lhs_; }
        [[nodiscard]] auto& op() const { return op_; }
        [[nodiscard]] auto& rhs() const { return *rhs_; }

    private:
        ExprPtr lhs_;
        Token op_;
        ExprPtr rhs_;
    };

    class ExprStmt;
    class PrintStmt;
    class VarDeclStmt;
    class BlockStmt;
    class IfStmt;
    class WhileStmt;

    class StmtVisitor
    {
    public:
        StmtVisitor() = default;
        virtual ~StmtVisitor() = default;

        virtual void visit(const ExprStmt& stmt) = 0;
        virtual void visit(const PrintStmt& stmt) = 0;
        virtual void visit(const VarDeclStmt& stmt) = 0;
        virtual void visit(const BlockStmt& stmt) = 0;
        virtual void visit(const IfStmt& stmt) = 0;
        virtual void visit(const WhileStmt& stmt) = 0;

    protected:
        StmtVisitor(const StmtVisitor&) = default;
        StmtVisitor(StmtVisitor&&) = default;
        StmtVisitor& operator=(const StmtVisitor&) = default;
        StmtVisitor& operator=(StmtVisitor&&) = default;
    };

    class Stmt
    {
    public:
        Stmt() = default;
        virtual ~Stmt() = default;

        virtual void accept(StmtVisitor& visitor) const = 0;

    protected:
        Stmt(const Stmt&) = default;
        Stmt(Stmt&&) = default;
        Stmt& operator=(const Stmt&) = default;
        Stmt& operator=(Stmt&&) = default;
    };

    using StmtPtr = std::unique_ptr<Stmt>;

    class ExprStmt : public Stmt
    {
    public:
        explicit ExprStmt(ExprPtr expr);

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] auto& expr() const { return *expr_; }

    private:
        ExprPtr expr_;
    };

    class PrintStmt : public Stmt
    {
    public:
        explicit PrintStmt(ExprPtr expr);

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] auto& expr() const { return *expr_; }

    private:
        ExprPtr expr_;
    };

    class VarDeclStmt : public Stmt
    {
    public:
        VarDeclStmt(Token identifier, ExprPtr initializer);

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] auto& identifier() const { return identifier_; }
        [[nodiscard]] auto* initializer() const { return initializer_.get(); }

    private:
        Token identifier_;
        ExprPtr initializer_;
    };

    class BlockStmt : public Stmt
    {
    public:
        explicit BlockStmt(std::vector<StmtPtr> statements);

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] auto& statements() const { return statements_; }

    private:
        std::vector<StmtPtr> statements_;
    };

    class IfStmt : public Stmt
    {
    public:
        IfStmt(ExprPtr condition, StmtPtr then_stmt, StmtPtr else_stmt);

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] auto& condition() const { return *condition_; }
        [[nodiscard]] auto& then_branch() const { return *then_stmt_; }
        [[nodiscard]] auto* else_branch() const { return else_stmt_.get(); }

    private:
        ExprPtr condition_;
        StmtPtr then_stmt_;
        StmtPtr else_stmt_;
    };

    class WhileStmt : public Stmt
    {
    public:
        WhileStmt(ExprPtr condition, StmtPtr body);

        void accept(StmtVisitor& visitor) const override { visitor.visit(*this); }

        [[nodiscard]] auto& condition() const { return *condition_; }
        [[nodiscard]] auto& body() const { return *body_; }

    private:
        ExprPtr condition_;
        StmtPtr body_;
    };
} // namespace lox
