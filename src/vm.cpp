#include "vm.h"

#include "bytecode.h"
#include "error.h"
#include "vm_instruction.h"

#include "lox_boolean.h"
#include "lox_nil.h"
#include "lox_number.h"
#include "lox_string.h"

#include <fmt/format.h>

#include <cassert>
#include <cstdio>
#include <cstring>

#define LOX_BINARY_OP(lhs, rhs, op, op_string)                        \
    auto result = lhs->op(rhs.get());                                 \
    if (!result) {                                                    \
        throw_unsupported_binary_op(op_string, lhs.get(), rhs.get()); \
    }                                                                 \
    push(std::move(result))

#define LOX_UNARY_OP(object, op, op_string)                  \
    auto result = object->op();                              \
    if (!result) {                                           \
        throw_unsupported_unary_op(op_string, object.get()); \
    }                                                        \
    push(std::move(result))

namespace lox
{
    void VM::execute(std::span<const std::uint8_t> code)
    {
        auto bytecode = Bytecode{code};
        // Extract constants
        constants_.clear();
        while (bytecode.peek() == '@') {
            bytecode.read(); // consume @
            const auto index = bytecode.read();
            const auto type = bytecode.read();
            switch (type) {
                case 'd':
                    constants_.push_back(std::make_shared<LoxNumber>(bytecode.read_number()));
                    break;
                case 's':
                    constants_.push_back(std::make_shared<LoxString>(bytecode.read_string()));
                    break;
            }
        }

        while (!bytecode.is_eof()) {
            const auto op = bytecode.fetch();
            switch (op) {
                case Instruction::Nop:
                    continue;
                case Instruction::Add:
                    op_add();
                    continue;
                case Instruction::Sub:
                    op_sub();
                    continue;
                case Instruction::Mul:
                    op_mul();
                    continue;
                case Instruction::Div:
                    op_div();
                    continue;
                case Instruction::Neg:
                    op_neg();
                    continue;
                case Instruction::Not:
                    op_not();
                    continue;
                case Instruction::PushConstant:
                    op_push_constant(bytecode.read());
                    continue;
                case Instruction::PushNil:
                    op_push_nil();
                    continue;
                case Instruction::PushTrue:
                    op_push_true();
                    continue;
                case Instruction::PushFalse:
                    op_push_false();
                    continue;
                case Instruction::Pop:
                    op_pop();
                    continue;
                case Instruction::Print:
                    op_print();
                    continue;
                case Instruction::DefineGlobal:
                    op_define_global(bytecode.read());
                    continue;
                case Instruction::SetGlobal:
                    op_set_global(bytecode.read());
                    continue;
                case Instruction::GetGlobal:
                    op_get_global(bytecode.read());
                    continue;
                case Instruction::SetLocal:
                    op_set_local(bytecode.read());
                    continue;
                case Instruction::GetLocal:
                    op_get_local(bytecode.read());
                    continue;
                case Instruction::Jmp:
                    bytecode.jump(bytecode.read_word());
                    continue;
                case Instruction::JmpFalse:
                    if (!peek()->is_truthy()) {
                        bytecode.jump(bytecode.read_word());
                    }
                    continue;;
                case Instruction::Trap:
                    throw VMTrap();
            }
            assert(false && "unhandled/invalid bytecode in VM::execute()");
        }
    }

    void VM::push(LoxObjectRef object)
    {
        stack_.push_back(std::move(object));
    }

    LoxObjectRef VM::pop()
    {
        assert(!stack_.empty());
        auto value = std::move(stack_.back());
        stack_.pop_back();
        return value;
    }

    LoxObjectRef VM::peek() const
    {
        assert(!stack_.empty());
        return stack_.back();
    }

    void VM::throw_unsupported_binary_op(const char* op, const LoxObject* lhs, const LoxObject* rhs) const
    {
        throw LoxError(fmt::format("unsupported binary operation {} with types '{}' & '{}'", op, lhs->type_name(), rhs->type_name()), current_location());
    }

    void VM::throw_unsupported_unary_op(const char* op, const LoxObject* object) const
    {
        throw LoxError(fmt::format("unsupported unary operation {} with type '{}'", op, object->type_name()), current_location());
    }

    void VM::throw_undefined_global(const std::string& identifier) const
    {
        throw LoxError(fmt::format("accessing undefined global '{}'", identifier), current_location());
    }

    void VM::op_add()
    {
        auto rhs = pop();
        auto lhs = pop();
        LOX_BINARY_OP(lhs, rhs, add, "add '+'");
    }

    void VM::op_sub()
    {
        auto rhs = pop();
        auto lhs = pop();
        LOX_BINARY_OP(lhs, rhs, subtract, "subtract '-'");
    }

    void VM::op_mul()
    {
        auto rhs = pop();
        auto lhs = pop();
        LOX_BINARY_OP(lhs, rhs, multiply, "multiply '*'");
    }

    void VM::op_div()
    {
        auto rhs = pop();
        auto lhs = pop();
        LOX_BINARY_OP(lhs, rhs, divide, "divide '/'");
    }

    void VM::op_neg()
    {
        LOX_UNARY_OP(pop(), negate, "negate '-'");
    }

    void VM::op_not()
    {
        auto object = pop();
        push(std::make_shared<LoxBoolean>(!object->is_truthy()));
    }

    void VM::op_push_constant(std::uint8_t index)
    {
        push(constants_[index]);
    }

    void VM::op_push_nil()
    {
        push(LoxNil::nil_ref());
    }

    void VM::op_push_true()
    {
        push(LoxBoolean::true_ref());
    }

    void VM::op_push_false()
    {
        push(LoxBoolean::false_ref());
    }

    void VM::op_pop()
    {
        pop();
    }

    void VM::op_print()
    {
        std::puts(pop()->to_string().c_str());
    }

    void VM::op_define_global(std::uint8_t index)
    {
        const auto identifier = constants_[index];
        globals_[identifier->to_string()] = pop();
    }

    void VM::op_set_global(std::uint8_t index)
    {
        const auto identifier = constants_[index]->to_string();
        if (auto iter = globals_.find(identifier); iter != globals_.end()) {
            iter->second = peek();
        } else {
            throw_undefined_global(identifier);
        }
    }

    void VM::op_get_global(std::uint8_t index)
    {
        const auto identifier = constants_[index]->to_string();
        if (auto global = globals_.find(identifier); global != globals_.end()) {
            push(global->second);
        } else {
            throw_undefined_global(identifier);
        }
    }

    void VM::op_set_local(std::uint8_t index)
    {
        assert(stack_.size() > index && "incorrect stack address for local");
        stack_[index] = peek();
    }

    void VM::op_get_local(std::uint8_t index)
    {
        assert(stack_.size() > index && "incorrect stack address for local");
        push(stack_[index]);
    }
} // namespace lox
