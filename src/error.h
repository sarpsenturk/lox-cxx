#pragma once

#include "source_location.h"

#include <exception>
#include <string>

namespace lox
{
    class LoxError : public std::exception
    {
    public:
        LoxError(const std::string& message, SourceLocation location);

        [[nodiscard]] auto& msg() const { return message_; }
        [[nodiscard]] auto& location() const { return location_; }

        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string message_;
        SourceLocation location_;
    };
} // namespace lox
