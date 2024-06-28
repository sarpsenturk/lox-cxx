#include "bytecode_compiler.h"
#include "ast.h"
#include "token.h"
#include "vm_instruction.h"

#include <array>
#include <bit>

namespace lox
{
    namespace
    {
        struct ConstantType {
            std::uint8_t operator()(NilLiteral) const { return '?'; }
            std::uint8_t operator()(NumberLiteral) const { return 'd'; }
            std::uint8_t operator()(const StringLiteral&) const { return 's'; }
            std::uint8_t operator()(BooleanLiteral) const { return 'b'; }
        };

        struct ConstantToBytes {
            std::vector<std::uint8_t> operator()(NilLiteral) const { return {}; }
            std::vector<std::uint8_t> operator()(NumberLiteral number) const
            {

                const auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(NumberLiteral)>>(number);
                return std::vector<std::uint8_t>{bytes.begin(), bytes.end()};
            }
            auto operator()(const StringLiteral& string) const
            {
                auto bytes = std::vector<std::uint8_t>{string.begin(), string.end()};
                bytes.push_back(0);
                return bytes;
            }
            std::vector<std::uint8_t> operator()(BooleanLiteral boolean) const { return {static_cast<std::uint8_t>(boolean)}; }
        };
    } // namespace

    CompileResult BytecodeCompiler::compile(const std::vector<StmtPtr>& statements)
    {
        try {
            for (const auto& stmt : statements) {
                compile(*stmt);
            }
            std::vector<std::uint8_t> bytecode;
            bytecode.reserve(code_.size() + constants_.size());
            bytecode.insert(bytecode.end(), constants_.begin(), constants_.end());
            bytecode.insert(bytecode.end(), code_.begin(), code_.end());
            return CompileOutput{bytecode};
        } catch (const CompileError& err) {
            return tl::unexpected(err);
        }
    }

    void BytecodeCompiler::compile(const Stmt& stmt)
    {
        stmt.accept(*this);
    }

    void BytecodeCompiler::compile(const Expr& expr)
    {
        expr.accept(*this);
    }

    void BytecodeCompiler::write_instruction(Instruction instruction)
    {
        code_.push_back(static_cast<std::uint8_t>(instruction));
    }

    void BytecodeCompiler::write_instruction(Instruction instruction, std::uint8_t operand)
    {
        code_.push_back(static_cast<std::uint8_t>(instruction));
        code_.push_back(operand);
    }

    void BytecodeCompiler::visit(const BinaryExpr& expr)
    {
        compile(expr.lhs());
        compile(expr.rhs());
        switch (expr.op().type) {
            case TokenType::Plus:
                write_instruction(Instruction::Add);
                break;
            case TokenType::Minus:
                write_instruction(Instruction::Sub);
                break;
            case TokenType::Star:
                write_instruction(Instruction::Mul);
                break;
            case TokenType::Slash:
                write_instruction(Instruction::Div);
                break;
        }
    }

    void BytecodeCompiler::visit(const UnaryExpr& expr)
    {
        compile(expr.expr());
        switch (expr.op().type) {
            case TokenType::Minus:
                write_instruction(Instruction::Neg);
                break;
            case TokenType::Bang:
                write_instruction(Instruction::Not);
                break;
        }
    }

    void BytecodeCompiler::visit(const ParenExpr& expr)
    {
        compile(expr.expr());
    }

    void BytecodeCompiler::visit(const LiteralExpr& expr)
    {
        if (std::get_if<NilLiteral>(&expr.literal())) {
            write_instruction(Instruction::PushNil);
        } else if (const auto* boolean = std::get_if<BooleanLiteral>(&expr.literal())) {
            write_instruction(*boolean ? Instruction::PushTrue : Instruction::PushFalse);
        } else {
            constants_.push_back('@');
            const auto constant_index = constant_index_++;
            constants_.push_back(constant_index);
            constants_.push_back(std::visit(ConstantType{}, expr.literal()));
            const auto bytes = std::visit(ConstantToBytes{}, expr.literal());
            constants_.insert(constants_.end(), bytes.begin(), bytes.end());
            write_instruction(Instruction::PushConstant, constant_index);
        }
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
        compile(stmt.expr());
    }

    void BytecodeCompiler::visit(const PrintStmt& stmt)
    {
        compile(stmt.expr());
        write_instruction(Instruction::Print);
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
