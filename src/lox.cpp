#include "lox.h"

#include "error.h"
#include "lexer.h"
#include "parser.h"

#include <cstdio>
#include <string>

#include <fmt/core.h>

namespace lox
{
    void Lox::run_file(const char* filename)
    {
        std::FILE* fp = std::fopen(filename, "r");
        if (fp == nullptr) {
            std::perror(filename);
            return;
        }

        std::fseek(fp, 0, SEEK_END);
        const auto size = std::ftell(fp);
        std::fseek(fp, 0, SEEK_SET);

        std::string source(size, ' ');
        if (std::fread(source.data(), sizeof(char), size, fp) < size) {
            if (std::ferror(fp)) {
                std::perror("I/O error");
                return;
            }
        }

        run_string(source);
    }

    void Lox::run_string(const std::string& source)
    {
        try {
            auto lexer = Lexer{source};
            const auto tokens = lexer.tokenize();

            auto parser = Parser{tokens};
            const auto parse_result = parser.parse();
            if (!parse_result.has_value()) {
                const auto& errors = parse_result.error();
                for (const auto& error : errors) {
                    fmt::println(stderr, "{}", error.what());
                }
                return;
            }

            const auto& statements = *parse_result;
        } catch (const LoxError& error) {
            fmt::println(stderr, "{}", error.what());
        }
    }
} // namespace lox
