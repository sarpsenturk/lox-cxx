#pragma once

#include "lox_object.h"

#include <memory>
#include <string>

namespace lox
{
    class LoxString : public LoxObject
    {
    public:
        LoxString(Token token, std::string value);

        [[nodiscard]] const char* type_name() const override { return "String"; }
        [[nodiscard]] std::string to_string() const override { return value_; }

        std::unique_ptr<LoxObject> add(const LoxObject* other) override;
        std::optional<bool> cmp_equal(const LoxObject* other) override;

    private:
        std::string value_;
    };
} // namespace lox
