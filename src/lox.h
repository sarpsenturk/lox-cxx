#pragma once

#include <string>

namespace lox
{
    class Lox
    {
    public:
        void run_file(const char* filename);
        void run_string(const std::string& string);
    };
} // namespace lox
