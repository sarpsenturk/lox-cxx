#include "lox_boolean.h"

#include "lox_nil.h"

namespace lox
{
    LoxBoolean::LoxBoolean(bool value)
        : value_(value)
    {
    }

    std::optional<bool> LoxBoolean::cmp_equal(const LoxObject* other)
    {
        // TODO: I'm not sure if Lox supports this kind of boolean checking.
        //  If not I will change it in the future
        if (dynamic_cast<const LoxNil*>(other)) {
            return false;
        }
        return value_ == other->is_truthy();
    }
} // namespace lox
