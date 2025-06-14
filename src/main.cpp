#include "Executor.hpp"
#include "Parser.hpp"
#include "tools.hpp"
#include <cstring>
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {

    if (argc > 1 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        printf(SOFTWARE_INFO);
        printf("Usage: rotvi [filename] "
               "    If no filename is provided, it will open a new file.\n");
        return 0;
    }

    DatabaseManager db_manager;

    Executor executor(db_manager);

    std::string input;

    namespace fs = std::filesystem;
    fs::create_directory("data");              // Ensure data directory exists
    std::filesystem::create_directory("data"); // Ensure data directory exists

    pwd = fs::current_path().string() + "/data/";

    std::cout << "Welcome to StuPidDB\n";
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