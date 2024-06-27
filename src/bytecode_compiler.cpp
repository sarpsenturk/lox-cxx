#include "bytecode_compiler.h"
#include "vm_instruction.h"


namespace lox
{
    CompileResult BytecodeCompiler::compile(const std::vector<StmtPtr>& statements)
    {
        try {
            for (const auto& stmt : statements) {
                compile(*stmt);
            }
            return CompileOutput{.bytecode = std::move(bytecode_)};
        } catch (const CompileError& err) {
            return tl::unexpected(err);
        }
    }

    void BytecodeCompiler::compile(const Stmt& stmt)
    {
        stmt.accept(*this);
    }

    void BytecodeCompiler::write_instruction(Instruction instruction)
    {
        bytecode_.push_back(static_cast<std::uint8_t>(instruction));
    }

    void BytecodeCompiler::visit(const BinaryExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const UnaryExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const ParenExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const LiteralExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const VarExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const AssignmentExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const LogicExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const CallExpr& expr)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const ExprStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const PrintStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const VarDeclStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const FunDeclStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const BlockStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const IfStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const WhileStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const ReturnStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }
} // namespace lox
