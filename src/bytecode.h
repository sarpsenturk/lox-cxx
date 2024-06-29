#pragma once

#include "vm_instruction.h"

#include <cstdint>
#include <span>
#include <string>

namespace lox
{
    class Bytecode
    {
    public:
        explicit Bytecode(std::span<const std::uint8_t> bytecode);

        std::uint8_t read();
        [[nodiscard]] std::uint8_t peek() const;
        Instruction fetch();
        double read_number();
        std::string read_string();
        [[nodiscard]] bool is_eof() const { return isp_ >= bytecode_.size(); }

    private:
        std::span<const std::uint8_t> bytecode_;
        std::size_t isp_ = 0;
    };
} // namespace lox
