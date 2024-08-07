#include "lox_boolean.h"

#include "lox_nil.h"

namespace lox
{
    LoxBoolean::LoxBoolean(bool value)
        : value_(value)
    {
    }

    LoxObjectRef LoxBoolean::true_ref()
    {
        static auto ref = std::make_shared<LoxBoolean>(true);
        return ref;
    }

    LoxObjectRef LoxBoolean::false_ref()
    {
        static auto ref = std::make_shared<LoxBoolean>(false);
        return ref;
    }

    LoxObjectRef LoxBoolean::get_ref(bool value)
    {
        return value ? true_ref() : false_ref();
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
