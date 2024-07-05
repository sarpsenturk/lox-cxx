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
        Trap = UINT8_MAX,
    };

    const char* format_as(Instruction instruction);
} // namespace lox
