#include "lox_object.h"

namespace lox
{
    LoxObject::LoxObject(Token token)
        : token_(token)
    {
    }

    std::unique_ptr<LoxObject> LoxObject::negate()
    {
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxObject::subtract(const LoxObject* other)
    {
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxObject::add(const LoxObject* other)
    {
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxObject::multiply(const LoxObject* other)
    {
        return nullptr;
    }

    std::unique_ptr<LoxObject> LoxObject::divide(const LoxObject* other)
    {
        return nullptr;
    }

    std::optional<bool> LoxObject::cmp_greater(const LoxObject* other)
    {
        return std::nullopt;
    }

    std::optional<bool> LoxObject::cmp_greater_equal(const LoxObject* other)
    {
        return std::nullopt;
    }

    std::optional<bool> LoxObject::cmp_less(const LoxObject* other)
    {
        return std::nullopt;
    }

    std::optional<bool> LoxObject::cmp_less_equal(const LoxObject* other)
    {
        return std::nullopt;
    }

    std::optional<bool> LoxObject::cmp_equal(const LoxObject* other)
    {
        return std::nullopt;
    }
} // namespace lox
