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
        void compile(const Expr& expr);

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
        void write_instruction(Instruction, std::uint8_t operand);
        void add_constant(double number);

        std::vector<std::uint8_t> code_;
        std::vector<std::uint8_t> constants_;
        std::uint8_t constant_index_ = 0;
    };
} // namespace lox
