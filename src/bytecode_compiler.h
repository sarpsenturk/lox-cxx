#pragma once

#include "ast.h"
#include "vm_instruction.h"

#include <tl/expected.hpp>

#include <cstdint>
#include <map>
#include <span>
#include <string>
#include <vector>

namespace lox
{
    struct CompileOutput {
        std::vector<std::uint8_t> bytecode;
    };

    struct CompileError {
        std::string message;
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

        void begin_scope();
        void end_scope();

    private:
        void write_instruction(Instruction instruction);
        void write_instruction(Instruction instruction, std::uint8_t operand);
        void write_instruction(Instruction instruction, std::uint8_t operand1, std::uint8_t operand2);

        std::size_t start_jump(Instruction jmp_instruction);
        void patch_jump(std::size_t offset);

        void do_loop(std::size_t loop_start);

        std::uint8_t add_constant(std::uint8_t type, std::span<const std::uint8_t> bytes);
        std::uint8_t constant_string(const StringLiteral& string);
        std::uint8_t constant_number(const NumberLiteral& number);
        int resolve_local(const Token& identifier);

        std::vector<std::uint8_t> code_;

        std::vector<std::uint8_t> constants_;
        std::uint8_t constant_index_ = 0;
        std::map<std::string, std::uint8_t> strings_;
        std::map<double, std::uint8_t> numbers_;

        std::map<std::string, std::uint8_t, std::less<>> globals_;

        struct LocalVar {
            std::string identifier;
            int depth;
        };

        std::vector<LocalVar> locals_;
        int scope_depth_ = 0;
    };
} // namespace lox
