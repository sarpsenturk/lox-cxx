#pragma once

#include <cstdint>

namespace lox
{
    struct SourceLocation {
        std::int32_t line;
        std::int32_t column;

        void new_line()
        {
            line += 1;
            column = 1;
        }

        constexpr bool operator==(const SourceLocation&) const = default;
    };
} // namespace lox
