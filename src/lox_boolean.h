#pragma once

#include "lox_object.h"

namespace lox
{
    class LoxBoolean : public LoxObject
    {
    public:
        LoxBoolean(Token token, bool value);

        [[nodiscard]] const char* type_name() const override { return "Boolean"; }
        [[nodiscard]] bool is_truthy() const override { return value_; }
        [[nodiscard]] std::string to_string() const override { return value_ ? "true" : "false"; }

        std::optional<bool> cmp_equal(const LoxObject* other) override;

    private:
        bool value_;
    };
} // namespace lox
