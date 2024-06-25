#pragma once

#include "ast.h"
#include "environment.h"
#include "lox_object.h"

#include <memory>
#include <span>

namespace lox
{
    class TreeWalkInterpreter
        : public StmtVisitor
        , public ExprVisitor
    {
    public:
        std::shared_ptr<LoxObject> evaluate(const Expr& expr);
        void execute(const Stmt& stmt);
        void execute(std::span<const StmtPtr> statements);

        void visit(const BinaryExpr& expr) override;
        void visit(const UnaryExpr& expr) override;
        void visit(const ParenExpr& expr) override;
        void visit(const LiteralExpr& expr) override;
        void visit(const VarExpr& expr) override;
        void visit(const AssignmentExpr& expr) override;
        void visit(const LogicExpr& expr) override;

        void visit(const ExprStmt& stmt) override;
        void visit(const PrintStmt& stmt) override;
        void visit(const VarDeclStmt& stmt) override;
        void visit(const BlockStmt& stmt) override;
        void visit(const IfStmt& stmt) override;
        void visit(const WhileStmt& stmt) override;

    private:
        std::shared_ptr<LoxObject> expr_result_ = nullptr;

        Environment environment_;
    };
} // namespace lox
