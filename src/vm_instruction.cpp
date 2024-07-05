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
            case Instruction::Not:
                return "not";
            case Instruction::PushConstant:
                return "push_constant";
            case Instruction::PushNil:
                return "push_nil";
            case Instruction::PushTrue:
                return "push_true";
            case Instruction::PushFalse:
                return "push_false";
            case Instruction::Pop:
                return "pop";
            case Instruction::Trap:
                return "trap";
            case Instruction::Print:
                return "print";
            case Instruction::DefineGlobal:
                return "define_global";
            case Instruction::SetGlobal:
                return "set_global";
            case Instruction::LoadGlobal:
                return "load_global";
        }
    }
} // namespace lox
