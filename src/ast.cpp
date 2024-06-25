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

    LogicExpr::LogicExpr(ExprPtr lhs, Token op, ExprPtr rhs)
        : lhs_(std::move(lhs))
        , op_(op)
        , rhs_(std::move(rhs))
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

    BlockStmt::BlockStmt(std::vector<StmtPtr> statements)
        : statements_(std::move(statements))
    {
    }

    IfStmt::IfStmt(ExprPtr condition, StmtPtr then_stmt, StmtPtr else_stmt)
        : condition_(std::move(condition))
        , then_stmt_(std::move(then_stmt))
        , else_stmt_(std::move(else_stmt))
    {
    }
} // namespace lox
