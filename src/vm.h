#pragma once

#include "error.h"
#include "lox_object.h"

#include <span>
#include <stack>
#include <vector>

namespace lox
{
    class VMTrap : public LoxError
    {
    public:
        VMTrap()
            : LoxError("Interpreter trap", {})
        {
        }
    };

    class VM
    {
    public:
        void execute(std::span<const std::uint8_t> code);

        void push(LoxObjectRef object);
        LoxObjectRef pop();

    private:
        [[noreturn]] void throw_unsupported_binary_op(const char* op, const LoxObject* lhs, const LoxObject* rhs) const;
        [[noreturn]] void throw_unsupported_unary_op(const char* op, const LoxObject* object) const;

        void op_add();
        void op_sub();
        void op_mul();
        void op_div();
        void op_neg();
        void op_not();
        void op_push_constant(std::uint8_t index);
        void op_push_nil();
        void op_push_true();
        void op_push_false();
        void op_pop();
        void op_print();

        std::stack<LoxObjectRef> stack_;
        std::vector<LoxObjectRef> constants_;
    };
} // namespace lox
