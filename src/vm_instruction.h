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
        PushConstant,
        PushNil,
        Trap = UINT8_MAX,
    };

    const char* format_as(Instruction instruction);
}

