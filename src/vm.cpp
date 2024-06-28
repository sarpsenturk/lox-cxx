#include "vm.h"

#include "vm_instruction.h"

#include "lox_nil.h"
#include "lox_number.h"
#include "lox_string.h"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace lox
{
    void VM::execute(std::span<const std::uint8_t> bytecode)
    {
        // Instruction pointer
        std::size_t isp = 0ull;
        auto read = [&]() -> std::uint8_t { return bytecode[isp++]; };
        auto peek = [&]() -> std::uint8_t { return bytecode[isp]; };
        auto fetch = [&]() -> Instruction { return static_cast<Instruction>(read()); };
        auto read_number = [&]() -> double {
            double value;
            std::memcpy(&value, &bytecode[isp], sizeof(double));
            isp += sizeof(double);
            return value;
        };
        auto read_string = [&]() -> std::string {
            std::string string;
            while (true) {
                const char c = read();
                if (c == '\0') {
                    break;
                }
                string.push_back(c);
            }
            return string;
        };

        // Extract constants
        constants_.clear();
        while (peek() == '@') {
            read(); // consume @
            const auto index = read();
            const auto type = read();
            switch (type) {
                case 'd':
                    constants_.push_back(std::make_shared<LoxNumber>(read_number()));
                    break;
                case 's':
                    constants_.push_back(std::make_shared<LoxString>(read_string()));
                    break;
            }
        }

        while (isp < bytecode.size()) {
            const auto op = fetch();
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
                case Instruction::PushConstant:
                    op_push_constant(read());
                    break;
                case Instruction::PushNil:
                    op_push_nil();
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

    void VM::op_push_constant(std::uint8_t index)
    {
        push(constants_[index]);
    }

    void VM::op_push_nil()
    {
        push(std::make_shared<LoxNil>());
    }

    void VM::op_print()
    {
        std::puts(pop()->to_string().c_str());
    }
} // namespace lox
