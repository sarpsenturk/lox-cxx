#include "environment.h"

namespace lox
{
    void Environment::define(std::string_view identifier, std::shared_ptr<LoxObject> object)
    {
        objects_.insert(std::make_pair(identifier, std::move(object)));
    }

    bool Environment::assign(std::string_view identifier, std::shared_ptr<LoxObject> value)
    {
        if (auto iter = objects_.find(identifier); iter != objects_.end()) {
            iter->second = std::move(value);
            return true;
        }
        return false;
    }

    std::shared_ptr<LoxObject> Environment::get(std::string_view name) const
    {
        if (auto iter = objects_.find(name); iter != objects_.end()) {
            return iter->second;
        }
        return nullptr;
    }
} // namespace lox
