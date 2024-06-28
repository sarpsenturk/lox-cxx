#pragma once

#include "lox_object.h"

#include <concepts>
#include <memory>
#include <vector>

namespace lox
{
    using ArgList = std::vector<LoxObjectRef>;

    class LoxCallable : public LoxObject
    {
    public:
        virtual std::shared_ptr<LoxObject> call(const std::vector<std::shared_ptr<LoxObject>>& args) = 0;
        virtual std::size_t arity() const = 0;
    };

    template<std::invocable<ArgList> F>
        requires std::convertible_to<std::invoke_result_t<F, ArgList>, LoxObjectRef>
    class LoxFunction : public LoxCallable
    {
    public:
        LoxFunction(std::size_t arity, auto&& function)
            : arity_(arity)
            , function_(std::forward<decltype(function)>(function))
        {
        }

        [[nodiscard]] const char* type_name() const override { return "function"; }
        [[nodiscard]] std::string to_string() const override { return name_; }

        std::shared_ptr<LoxObject> call(const std::vector<std::shared_ptr<LoxObject>>& args) override { return function_(args); }
        [[nodiscard]] std::size_t arity() const override { return arity_; }

    private:
        int arity_;
        F function_;
        std::string name_;
    };
} // namespace lox
