#include "Executor.hpp"
#include "Parser.hpp"
#include <filesystem>
#include <iostream>
#include <memory>

int main() {
    DatabaseManager db_manager;
    Executor executor(db_manager);
    std::string input;
    namespace fs = std::filesystem;
    fs::create_directory("data");              // Ensure data directory exists
    std::filesystem::create_directory("data"); // Ensure data directory exists
    std::cout << "Welcome to MicroDB\n";
    while (std::cout << "> ", std::getline(std::cin, input)) {
        if (input == "exit")
            break;
        try {
            auto cmd = Parser::parse(input);
            executor.execute(*cmd);
        } catch (const std::exception &e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
    std::cout << "Goodbye\n";
    return 0;
}