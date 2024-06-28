#pragma once

#include "lox_object.h"

namespace lox
{
    class LoxNil : public LoxObject
    {
    public:
        [[nodiscard]] const char* type_name() const override { return "Nil"; }
        [[nodiscard]] bool is_truthy() const override { return false; }
        [[nodiscard]] std::string to_string() const override { return "nil"; }

        std::optional<bool> cmp_equal(const LoxObject* other) override;
    };
} // namespace lox
