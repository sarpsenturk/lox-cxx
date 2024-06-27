#pragma once

#include "vm.h"

#include <string>

namespace lox
{
    class Lox
    {
    public:
        void run_file(const char* filename);
        void run_string(const std::string& string);

    private:
        VM vm_;
    };
} // namespace lox
