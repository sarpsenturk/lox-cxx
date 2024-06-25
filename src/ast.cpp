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

    CallExpr::CallExpr(ExprPtr calle, std::vector<ExprPtr> arguments, Token call_end)
        : calle_(std::move(calle))
        , arguments_(std::move(arguments))
        , call_end_(call_end)
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

    FunDeclStmt::FunDeclStmt(Token identifier, std::vector<Token> parameters, std::unique_ptr<BlockStmt> body)
        : identifier_(identifier)
        , parameters_(std::move(parameters))
        , body_(std::move(body))
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

    WhileStmt::WhileStmt(ExprPtr condition, StmtPtr body)
        : condition_(std::move(condition))
        , body_(std::move(body))
    {
    }
} // namespace lox
