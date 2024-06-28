#pragma once

#include "lox_object.h"

namespace lox
{
    class LoxBoolean : public LoxObject
    {
    public:
        explicit LoxBoolean(bool value);

        static LoxObjectRef true_ref();
        static LoxObjectRef false_ref();

        [[nodiscard]] const char* type_name() const override { return "Boolean"; }
        [[nodiscard]] bool is_truthy() const override { return value_; }
        [[nodiscard]] std::string to_string() const override { return value_ ? "true" : "false"; }

        std::optional<bool> cmp_equal(const LoxObject* other) override;

    private:
        bool value_;
    };
} // namespace lox
