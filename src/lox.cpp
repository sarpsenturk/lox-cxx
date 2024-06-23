#include "lox.h"

#include <iostream>

namespace lox
{
    void Lox::run_file(const char* filename)
    {
        std::cout << "Run file" << filename << '\n';
    }

    void Lox::run_string(const std::string& string)
    {
        std::cout << "Run string " << string << '\n';
    }
} // namespace lox
