#pragma once

#include "tree_walk.h"

#include <string>

namespace lox
{
    class Lox
    {
    public:
        void run_file(const char* filename);
        void run_string(const std::string& string);

    private:
        TreeWalkInterpreter interpreter_;
    };
} // namespace lox
