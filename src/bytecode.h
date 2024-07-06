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
        std::uint16_t read_word();
        std::int16_t read_signed_word();
        [[nodiscard]] std::uint8_t peek() const;
        Instruction fetch();
        double read_number();
        std::string read_string();

        void jump(std::uint16_t offset);
        void jump_signed(std::int16_t offset);

        [[nodiscard]] bool is_eof() const { return isp_ >= bytecode_.size(); }

    private:
        std::span<const std::uint8_t> bytecode_;
        std::size_t isp_ = 0;
    };
} // namespace lox
