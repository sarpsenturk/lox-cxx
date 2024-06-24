#include "lox_string.h"

namespace lox
{
    LoxString::LoxString(Token token, std::string value)
        : LoxObject(token)
        , value_(std::move(value))
    {
    }

    std::unique_ptr<LoxObject> LoxString::add(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxString*>(other)) {
            return std::make_unique<LoxString>(token(), value_ + rhs->value_);
        }
        return nullptr;
    }

    std::optional<bool> LoxString::cmp_equal(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxString*>(other)) {
            return value_ == rhs->value_;
        }
        return std::nullopt;
    }
} // namespace lox
