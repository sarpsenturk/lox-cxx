#include "vm.h"

#include "vm_instruction.h"

#include <cassert>

namespace lox
{
    void VM::execute(std::span<const std::uint8_t> bytecode)
    {
        bytecode_ = bytecode;
        while (true) {
            const auto op = fetch();
            switch (op) {
                case Instruction::Nop:
                    continue;
                case Instruction::Trap:
                    throw VMTrap();
            }
        }
    }

    Instruction VM::fetch()
    {
        assert(isp_ < bytecode_.size());
        return static_cast<Instruction>(bytecode_[isp_++]);
    }
} // namespace lox
