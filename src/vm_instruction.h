#pragma once

#include <cstdint>

namespace lox
{
    enum class Instruction : std::uint8_t {
        Nop = 0,
        Add,
        Sub,
        Mul,
        Div,
        Neg,
        Not,
        Less,
        Greater,
        Equal,
        PushConstant,
        PushNil,
        PushTrue,
        PushFalse,
        Pop,
        Print,
        DefineGlobal,
        SetGlobal,
        GetGlobal,
        SetLocal,
        GetLocal,
        Jmp,
        JmpFalse,
        JmpTrue,
        JmpSigned,
        Trap = UINT8_MAX,
    };

    const char* format_as(Instruction instruction);
} // namespace lox
