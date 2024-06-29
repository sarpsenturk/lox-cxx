#include "vm.h"

#include "vm_instruction.h"
#include "bytecode.h"

#include "lox_boolean.h"
#include "lox_nil.h"
#include "lox_number.h"
#include "lox_string.h"

#include <cassert>
#include <cstdio>
#include <cstring>

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
                    break;
                case Instruction::Sub:
                    op_sub();
                    break;
                case Instruction::Mul:
                    op_mul();
                    break;
                case Instruction::Div:
                    op_div();
                    break;
                case Instruction::Neg:
                    op_neg();
                    break;
                case Instruction::Not:
                    op_not();
                    break;
                case Instruction::PushConstant:
                    op_push_constant(bytecode.read());
                    break;
                case Instruction::PushNil:
                    op_push_nil();
                    break;
                case Instruction::PushTrue:
                    op_push_true();
                    break;
                case Instruction::PushFalse:
                    op_push_false();
                    break;
                case Instruction::Print:
                    op_print();
                    break;
                case Instruction::Trap:
                    throw VMTrap();
            }
        }
    }

    void VM::push(LoxObjectRef object)
    {
        stack_.push(std::move(object));
    }

    LoxObjectRef VM::pop()
    {
        assert(!stack_.empty());
        auto value = std::move(stack_.top());
        stack_.pop();
        return value;
    }

    void VM::op_add()
    {
        auto rhs = pop();
        auto lhs = pop();
        push(lhs->add(rhs.get()));
    }

    void VM::op_sub()
    {
        auto rhs = pop();
        auto lhs = pop();
        push(lhs->subtract(rhs.get()));
    }

    void VM::op_mul()
    {
        auto rhs = pop();
        auto lhs = pop();
        push(lhs->multiply(rhs.get()));
    }

    void VM::op_div()
    {
        auto rhs = pop();
        auto lhs = pop();
        push(lhs->divide(rhs.get()));
    }

    void VM::op_neg()
    {
        push(pop()->negate());
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

    void VM::op_print()
    {
        std::puts(pop()->to_string().c_str());
    }
} // namespace lox
