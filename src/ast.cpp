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

    VarExpr::VarExpr(Token identifier)
        : identifier_(identifier)
    {
    }

    AssignmentExpr::AssignmentExpr(Token identifier, ExprPtr value)
        : identifier_(identifier)
        , value_(std::move(value))
    {
    }

    ExprStmt::ExprStmt(ExprPtr expr)
        : expr_(std::move(expr))
    {
    }

    PrintStmt::PrintStmt(ExprPtr expr)
        : expr_(std::move(expr))
    {
    }

    VarDeclStmt::VarDeclStmt(Token identifier, ExprPtr initializer)
        : identifier_(identifier)
        , initializer_(std::move(initializer))
    {
    }
} // namespace lox
