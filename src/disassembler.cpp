#include "disassembler.h"

#include "bytecode.h"
#include "vm_instruction.h"

#include <fmt/format.h>

#include <sstream>

namespace lox
{
    std::string disassemble(std::span<const std::uint8_t> code)
    {
        std::stringstream result;
        auto bytecode = Bytecode{code};

        // Extract constants
        while (bytecode.peek() == '@') {
            bytecode.read(); // consume @
            const auto index = bytecode.read();
            const auto type = bytecode.read();
            switch (type) {
                case 'd':
                    result << fmt::format("@{}{} {}\n", index, (char)type, bytecode.read_number());
                    break;
                case 's':
                    result << fmt::format("@{}{} \"{}\"\n", index, (char)type, bytecode.read_string());
                    break;
            }
        }

        auto format_default = [](Instruction instruction) { return fmt::format("{}\n", instruction); };
        auto format_operand = [](Instruction instruction, auto operand) { return fmt::format("{} {}\n", instruction, operand); };

        while (!bytecode.is_eof()) {
            const auto op = bytecode.fetch();
            switch (op) {
                case Instruction::PushConstant:
                case Instruction::DefineGlobal:
                case Instruction::SetGlobal:
                case Instruction::GetGlobal:
                case Instruction::SetLocal:
                case Instruction::GetLocal:
                    result << format_operand(op, bytecode.read());
                    break;
                case Instruction::Jmp:
                case Instruction::JmpFalse:
                    result << format_operand(op, bytecode.read_word());
                    break;
                case Instruction::Nop:
                case Instruction::Add:
                case Instruction::Sub:
                case Instruction::Mul:
                case Instruction::Div:
                case Instruction::Neg:
                case Instruction::Not:
                case Instruction::PushNil:
                case Instruction::PushTrue:
                case Instruction::PushFalse:
                case Instruction::Pop:
                case Instruction::Print:
                case Instruction::Trap:
                    result << format_default(op);
                    break;
            }
        }

        return result.str();
    }
} // namespace lox
