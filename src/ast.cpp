#include "ast.h"

namespace lox
{
    BinaryExpr::BinaryExpr(ExprPtr lhs, Token op, ExprPtr rhs)
        : lhs_(std::move(lhs))
        , op_(op)
        , rhs_(std::move(rhs))
    {
    }

    UnaryExpr::UnaryExpr(Token op, ExprPtr expr)
        : op_(op)
        , expr_(std::move(expr))
    {
    }

    ParenExpr::ParenExpr(ExprPtr expr)
        : expr_(std::move(expr))
    {
    }

    LiteralExpr::LiteralExpr(Token literal, LiteralValue value)
        : literal_(literal)
        , value_(std::move(value))
    {
    }
} // namespace lox
