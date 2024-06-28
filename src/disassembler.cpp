#include "disassembler.h"

#include "vm_instruction.h"

#include <fmt/format.h>

#include <sstream>

namespace lox
{
    std::string disassemble(std::span<const std::uint8_t> bytecode)
    {

        // Instruction pointer
        std::size_t isp = 0ull;
        auto read = [&]() -> std::uint8_t { return bytecode[isp++]; };
        auto peek = [&]() -> std::uint8_t { return bytecode[isp]; };
        auto fetch = [&]() -> Instruction { return static_cast<Instruction>(read()); };
        auto read_number = [&]() -> double {
            double value;
            std::memcpy(&value, &bytecode[isp], sizeof(double));
            isp += sizeof(double);
            return value;
        };
        auto read_string = [&]() -> std::string {
            std::string string;
            while (true) {
                const char c = read();
                if (c == '\0') {
                    break;
                }
                string.push_back(c);
            }
            return string;
        };

        std::stringstream result;

        // Extract constants
        while (peek() == '@') {
            read(); // consume @
            const auto index = read();
            const auto type = read();
            switch (type) {
                case 'd':
                    result << fmt::format("@{}{} {}\n", index, (char)type, read_number());
                    break;
                case 's':
                    result << fmt::format("@{}{} \"{}\"\n", index, (char)type, read_string());
                    break;
            }
        }

        auto format_default = [](Instruction instruction) { return fmt::format("{}\n", instruction); };
        auto format_operand = [](Instruction instruction, std::uint8_t operand) { return fmt::format("{} {}\n", instruction, operand); };

        while (isp < bytecode.size()) {
            const auto op = fetch();
            switch (op) {
                case Instruction::PushConstant:
                    result << format_operand(op, read());
                    break;
                default:
                    result << format_default(op);
                    break;
            }
        }

        return result.str();
    }
} // namespace lox
