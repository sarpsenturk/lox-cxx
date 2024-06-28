#include "lox_number.h"

#include "error.h"

#include <fmt/format.h>

namespace lox
{
    LoxNumber::LoxNumber(double value)
        : value_(value)
    {
    }

    std::string LoxNumber::to_string() const
    {
        return fmt::format("{}", value_);
    }

    std::unique_ptr<LoxObject> LoxNumber::negate()
    {
        return std::make_unique<LoxNumber>(value_);
    }

    std::unique_ptr<LoxObject> LoxNumber::subtract(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return std::make_unique<LoxNumber>(value_ - rhs->value_);
        }
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxNumber::add(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return std::make_unique<LoxNumber>(value_ + rhs->value_);
        }
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxNumber::multiply(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return std::make_unique<LoxNumber>(value_ * rhs->value_);
        }
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxNumber::divide(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            if (rhs->value_ == 0.0) {
                throw LoxError{"divide by 0", {}};
            }
            return std::make_unique<LoxNumber>(value_ / rhs->value_);
        }
        return nullptr;
    }

    std::optional<bool> LoxNumber::cmp_greater(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return value_ > rhs->value_;
        }
        return std::nullopt;
    }

    std::optional<bool> LoxNumber::cmp_greater_equal(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return value_ >= rhs->value_;
        }
        return std::nullopt;
    }

    std::optional<bool> LoxNumber::cmp_less(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return value_ < rhs->value_;
        }
        return std::nullopt;
    }

    std::optional<bool> LoxNumber::cmp_less_equal(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return value_ <= rhs->value_;
        }
        return std::nullopt;
    }

    std::optional<bool> LoxNumber::cmp_equal(const LoxObject* other)
    {
        if (const auto* rhs = dynamic_cast<const LoxNumber*>(other)) {
            return value_ == rhs->value_;
        }
        return std::nullopt;
    }
} // namespace lox
