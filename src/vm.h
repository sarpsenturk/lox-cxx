#pragma once

#include "error.h"
#include "vm_instruction.h"

#include <span>

namespace lox
{
    class VMTrap : public LoxError
    {
    public:
        VMTrap()
            : LoxError("Interpreter trap", {})
        {
        }
    };

    class VM
    {
    public:
        void execute(std::span<const std::uint8_t> bytecode);

    private:
        Instruction fetch();

        std::span<const std::uint8_t> bytecode_;
        std::size_t isp_ = 0;
    };
} // namespace lox
