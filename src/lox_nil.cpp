#include "lox_nil.h"

namespace lox
{
    std::optional<bool> LoxNil::cmp_equal(const LoxObject* other)
    {
        // All nil values are considered equivalent
        // and nil is not equivalent to anything else (such as false)
        return dynamic_cast<const LoxNil*>(other) != nullptr;
    }
} // namespace lox
