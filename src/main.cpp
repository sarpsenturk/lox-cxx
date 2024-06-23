#include "lox.h"

#include <iostream>
#include <string>

int main(int argc, const char* argv[])
{
    if (argc > 2) {
        std::cerr << "Usage: lox-cxx [filename]\n";
        return 1;
    }

    auto lox_engine = lox::Lox{};
    if (argc == 2) {
        lox_engine.run_file(argv[1]);
    } else if (argc == 1) {
        std::string input;
        do {
            std::cout << "> ";
            if (!std::getline(std::cin, input) || input == "exit") {
                break;
            }
            lox_engine.run_string(input);
        } while (true);
    }
    return 0;
}
