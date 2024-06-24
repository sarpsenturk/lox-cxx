#pragma once

#include "ast.h"
#include "lox_object.h"

#include <memory>

namespace lox
{
    class TreeWalkInterpreter : public ExprVisitor
    {
    public:
        std::unique_ptr<LoxObject> evaluate(const Expr& expr);

        void visit(const BinaryExpr& expr) override;
        void visit(const UnaryExpr& expr) override;
        void visit(const ParenExpr& expr) override;
        void visit(const LiteralExpr& expr) override;

    private:
        std::unique_ptr<LoxObject> expr_result_ = nullptr;
    };
} // namespace lox
