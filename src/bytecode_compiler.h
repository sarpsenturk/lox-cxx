#pragma once

#include "ast.h"
#include "vm_instruction.h"

#include <tl/expected.hpp>

#include <cstdint>
#include <vector>

namespace lox
{
    struct CompileOutput {
        std::vector<std::uint8_t> bytecode;
    };

    struct CompileError {
    };

    using CompileResult = tl::expected<CompileOutput, CompileError>;

    class BytecodeCompiler
        : public ExprVisitor
        , public StmtVisitor
    {
    public:
        CompileResult compile(const std::vector<StmtPtr>& statements);
        void compile(const Stmt& stmt);

        void visit(const BinaryExpr& expr) override;
        void visit(const UnaryExpr& expr) override;
        void visit(const ParenExpr& expr) override;
        void visit(const LiteralExpr& expr) override;
        void visit(const VarExpr& expr) override;
        void visit(const AssignmentExpr& expr) override;
        void visit(const LogicExpr& expr) override;
        void visit(const CallExpr& expr) override;

        void visit(const ExprStmt& stmt) override;
        void visit(const PrintStmt& stmt) override;
        void visit(const VarDeclStmt& stmt) override;
        void visit(const FunDeclStmt& stmt) override;
        void visit(const BlockStmt& stmt) override;
        void visit(const IfStmt& stmt) override;
        void visit(const WhileStmt& stmt) override;
        void visit(const ReturnStmt& stmt) override;

    private:
        void write_instruction(Instruction instruction);

        std::vector<std::uint8_t> bytecode_;
    };
} // namespace lox
