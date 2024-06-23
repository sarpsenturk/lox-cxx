#include "error.h"

#include <fmt/format.h>

namespace lox
{
    LoxError::LoxError(const std::string& message, SourceLocation location)
        : message_(fmt::format("[{}:{}] Error: {}", location.line, location.column, message))
        , location_(location)
    {
    }

    const char* LoxError::what() const
    {
        return message_.c_str();
    }
} // namespace lox
