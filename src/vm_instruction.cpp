#include "vm_instruction.h"

namespace lox
{
    const char* format_as(Instruction instruction)
    {
        switch (instruction) {
            case Instruction::Nop:
                return "nop";
            case Instruction::Add:
                return "add";
            case Instruction::Sub:
                return "sub";
            case Instruction::Mul:
                return "mul";
            case Instruction::Div:
                return "div";
            case Instruction::Neg:
                return "neg";
            case Instruction::PushConstant:
                return "push_constant";
            case Instruction::PushNil:
                return "push_nil";
            case Instruction::Trap:
                return "trap";
        }
    }
} // namespace lox
