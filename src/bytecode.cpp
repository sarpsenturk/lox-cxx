#include "bytecode.h"

#include <cassert>

namespace lox
{
    Bytecode::Bytecode(std::span<const std::uint8_t> bytecode)
        : bytecode_(bytecode)
    {
    }

    std::uint8_t Bytecode::read()
    {
        assert(!is_eof());
        return bytecode_[isp_++];
    }

    std::uint16_t Bytecode::read_word()
    {
        assert(!is_eof());
        std::uint16_t word;
        std::memcpy(&word, &bytecode_[isp_], sizeof(word));
        isp_ += sizeof(word);
        return word;
    }

    std::uint8_t Bytecode::peek() const
    {
        assert(!is_eof());
        return bytecode_[isp_];
    }

    Instruction Bytecode::fetch()
    {
        return static_cast<Instruction>(read());
    }

    double Bytecode::read_number()
    {
        double value;
        std::memcpy(&value, &bytecode_[isp_], sizeof(double));
        isp_ += sizeof(double);
        return value;
    }

    std::string Bytecode::read_string()
    {
        std::string string;
        while (true) {
            const char c = read();
            if (c == '\0') {
                break;
            }
            string.push_back(c);
        }
        return string;
    }

    void Bytecode::jump(std::uint16_t offset)
    {
        assert(isp_ + offset <= bytecode_.size());
        isp_ += offset;
    }
} // namespace lox
