#pragma once

#include "error.h"
#include "lox_object.h"

#include <map>
#include <span>
#include <string>
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
        LoxObjectRef peek() const;

    private:
        [[noreturn]] void throw_unsupported_binary_op(const char* op, const LoxObject* lhs, const LoxObject* rhs) const;
        [[noreturn]] void throw_unsupported_unary_op(const char* op, const LoxObject* object) const;
        [[noreturn]] void throw_undefined_global(const std::string& identifier) const;

        [[nodiscard]] SourceLocation current_location() const { return {}; }

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
        void op_define_global(std::uint8_t index);
        void op_set_global(std::uint8_t index);
        void op_get_global(std::uint8_t index);
        void op_set_local(std::uint8_t index);
        void op_get_local(std::uint8_t index);

        std::vector<LoxObjectRef> stack_;
        std::vector<LoxObjectRef> constants_;
        std::map<std::string, LoxObjectRef, std::less<>> globals_;
    };
} // namespace lox
