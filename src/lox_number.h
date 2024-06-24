#pragma once

#include "lox_object.h"

#include <memory>

namespace lox
{
    class LoxNumber final : public LoxObject
    {
    public:
        LoxNumber(Token token, double value);

        [[nodiscard]] const char* type_name() const override { return "Number"; }
        [[nodiscard]] bool is_truthy() const override { return value_ != 0.0; };
        [[nodiscard]] std::string to_string() const override;

        std::unique_ptr<LoxObject> negate() override;
        std::unique_ptr<LoxObject> subtract(const LoxObject* other) override;
        std::unique_ptr<LoxObject> add(const LoxObject* other) override;
        std::unique_ptr<LoxObject> multiply(const LoxObject* other) override;
        std::unique_ptr<LoxObject> divide(const LoxObject* other) override;
        std::optional<bool> cmp_greater(const LoxObject* other) override;
        std::optional<bool> cmp_greater_equal(const LoxObject* other) override;
        std::optional<bool> cmp_less(const LoxObject* other) override;
        std::optional<bool> cmp_less_equal(const LoxObject* other) override;
        std::optional<bool> cmp_equal(const LoxObject* other) override;

    private:
        double value_;
    };
} // namespace lox
