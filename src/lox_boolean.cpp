#include "lox_boolean.h"

namespace lox
{
    LoxBoolean::LoxBoolean(Token token, bool value)
        : LoxObject(token)
        , value_(value)
    {
    }

    std::optional<bool> LoxBoolean::cmp_equal(const LoxObject* other)
    {
        // TODO: I'm not sure if Lox supports this kind of boolean checking.
        //  If not I will change it in the future
        return value_ == other->is_truthy();
    }
} // namespace lox
