#pragma once

#include "token.h"

#include <memory>
#include <optional>
#include <string>

namespace lox
{
    class LoxObject
    {
    public:
        explicit LoxObject(Token token);
        virtual ~LoxObject() = default;

        [[nodiscard]] virtual const char* type_name() const = 0;
        [[nodiscard]] virtual bool is_truthy() const { return true; }
        // TODO: Return a reference so objects can cache string formatting results
        [[nodiscard]] virtual std::string to_string() const = 0;

        virtual std::unique_ptr<LoxObject> negate();
        virtual std::unique_ptr<LoxObject> subtract(const LoxObject* other);
        virtual std::unique_ptr<LoxObject> add(const LoxObject* other);
        virtual std::unique_ptr<LoxObject> multiply(const LoxObject* other);
        virtual std::unique_ptr<LoxObject> divide(const LoxObject* other);
        virtual std::optional<bool> cmp_greater(const LoxObject* other);
        virtual std::optional<bool> cmp_greater_equal(const LoxObject* other);
        virtual std::optional<bool> cmp_less(const LoxObject* other);
        virtual std::optional<bool> cmp_less_equal(const LoxObject* other);
        virtual std::optional<bool> cmp_equal(const LoxObject* other);

        [[nodiscard]] auto& token() const { return token_; }

    protected:
        LoxObject(const LoxObject&) = default;
        LoxObject(LoxObject&&) = default;
        LoxObject& operator=(const LoxObject&) = default;
        LoxObject& operator=(LoxObject&&) = default;

    private:
        Token token_;
    };
} // namespace lox
