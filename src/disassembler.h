#pragma once

#include <cstdint>
#include <span>
#include <string>

namespace lox
{
    std::string disassemble(std::span<const std::uint8_t> code);
}
