#pragma once

#include "lox_object.h"

#include <map>
#include <string>
#include <string_view>

namespace lox
{
    class Environment
    {
    public:
        Environment() = default;
        explicit Environment(Environment* parent);

        void define(std::string_view identifier, std::shared_ptr<LoxObject> object);
        bool assign(std::string_view identifier, std::shared_ptr<LoxObject> value);
        [[nodiscard]] std::shared_ptr<LoxObject> get(std::string_view name) const;

    private:
        std::map<std::string, std::shared_ptr<LoxObject>, std::less<>> objects_;
        Environment* parent_ = nullptr;
    };
} // namespace lox
