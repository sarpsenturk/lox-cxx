#pragma once

#include <cstdint>

namespace lox
{
    struct SourceLocation {
        std::int32_t line;
        std::int32_t column;
    };
} // namespace lox
