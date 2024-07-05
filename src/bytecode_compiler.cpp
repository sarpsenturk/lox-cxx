#include "bytecode_compiler.h"
#include "ast.h"
#include "token.h"
#include "vm_instruction.h"

#include <fmt/format.h>

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <ranges>

namespace lox
{
    namespace
    {
        struct Scope {
            BytecodeCompiler* compiler;

            Scope(BytecodeCompiler* compiler)
                : compiler(compiler)
            {
                compiler->begin_scope();
            }

            ~Scope()
            {
                compiler->end_scope();
            }
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

    std::uint8_t BytecodeCompiler::add_constant(std::uint8_t type, std::span<const std::uint8_t> bytes)
    {
        if (constants_.size() >= UINT8_MAX) {
            throw CompileError("can't have more than 256 constants");
        }
        constants_.push_back('@');
        const auto constant_index = constant_index_++;
        constants_.push_back(constant_index);
        constants_.push_back(type);
        constants_.insert(constants_.end(), bytes.begin(), bytes.end());
        return constant_index;
    }

    std::uint8_t BytecodeCompiler::constant_string(const StringLiteral& string)
    {
        if (auto iter = strings_.find(string); iter != strings_.end()) {
            return iter->second;
        }
        auto bytes = std::vector<std::uint8_t>{string.begin(), string.end()};
        bytes.push_back(0);
        const auto index = add_constant('s', bytes);
        strings_.insert(std::make_pair(string, index));
        return index;
    }

    std::uint8_t BytecodeCompiler::constant_number(const NumberLiteral& number)
    {
        if (auto iter = numbers_.find(number); iter != numbers_.end()) {
            return iter->second;
        }
        const auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(NumberLiteral)>>(number);
        const auto index = add_constant('d', bytes);
        numbers_.insert(std::make_pair(number, index));
        return index;
    }

    void BytecodeCompiler::begin_scope()
    {
        scope_depth_ += 1;
    }

    void BytecodeCompiler::end_scope()
    {
        scope_depth_ -= 1;
        while (!locals_.empty() && locals_.back().depth > scope_depth_) {
            write_instruction(Instruction::Pop);
            locals_.pop_back();
        }
    }

    int BytecodeCompiler::resolve_local(const Token& identifier)
    {
        auto iter = std::ranges::find(std::views::reverse(locals_), identifier.lexeme, &LocalVar::identifier);
        if (iter == locals_.rend()) {
            return -1;
        }

        return static_cast<int>(std::distance(locals_.begin(), iter.base()) - 1);
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
        } else if (const auto* string = std::get_if<StringLiteral>(&expr.literal())) {
            const auto index = constant_string(*string);
            write_instruction(Instruction::PushConstant, index);
        } else if (const auto* number = std::get_if<NumberLiteral>(&expr.literal())) {
            const auto index = constant_number(*number);
            write_instruction(Instruction::PushConstant, index);
        }
    }

    void BytecodeCompiler::visit(const VarExpr& expr)
    {
        if (auto local = resolve_local(expr.identifier()); local != -1) {
            write_instruction(Instruction::GetLocal, local);
        } else {
            const auto global = constant_string(StringLiteral{expr.identifier().lexeme});
            write_instruction(Instruction::GetGlobal, global);
        }
    }

    void BytecodeCompiler::visit(const AssignmentExpr& expr)
    {
        compile(expr.value());

        if (auto local = resolve_local(expr.identifier()); local != -1) {
            write_instruction(Instruction::SetLocal, local);
        } else {
            const auto global = constant_string(StringLiteral{expr.identifier().lexeme});
            write_instruction(Instruction::SetGlobal, global);
        }
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
        write_instruction(Instruction::Pop);
    }

    void BytecodeCompiler::visit(const PrintStmt& stmt)
    {
        compile(stmt.expr());
        write_instruction(Instruction::Print);
    }

    void BytecodeCompiler::visit(const VarDeclStmt& stmt)
    {
        if (scope_depth_ > 0) {
            for (auto iter = locals_.rbegin(); iter != locals_.rend(); ++iter) {
                if (iter->depth != -1 && iter->depth < scope_depth_) {
                    break;
                }
                if (stmt.identifier().lexeme == iter->identifier && iter->depth == scope_depth_) {
                    throw CompileError(fmt::format("redefinition of local variable '{}' is not allowed", iter->identifier));
                }
            }
            locals_.emplace_back(std::string{stmt.identifier().lexeme}, -1); // Mark uninitialized
        }

        if (const auto* initializer = stmt.initializer()) {
            compile(*initializer);
        } else {
            write_instruction(Instruction::PushNil);
        }

        if (scope_depth_ > 0) {
            locals_.back().depth = scope_depth_; // Mark initialized
        } else {
            const auto name = constant_string(StringLiteral{stmt.identifier().lexeme});
            write_instruction(Instruction::DefineGlobal, name);
        }
    }

    void BytecodeCompiler::visit(const FunDeclStmt& stmt)
    {
        write_instruction(Instruction::Trap);
    }

    void BytecodeCompiler::visit(const BlockStmt& stmt)
    {
        const auto scope = Scope{this};
        for (const auto& statement : stmt.statements()) {
            compile(*statement);
        }
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
