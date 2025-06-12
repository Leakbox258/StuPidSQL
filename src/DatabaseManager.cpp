#include "DatabaseManager.hpp"
#include "Database.hpp"
#include <filesystem>
#include <iostream>
#include <stdexcept>

DatabaseManager::DatabaseManager() : current_db(nullptr) {}

DatabaseManager::~DatabaseManager() {}

void DatabaseManager::createDatabase(const std::string &dbname) {
    std::filesystem::path db_path = "data/" + dbname;
    if (std::filesystem::exists(db_path)) {
        throw std::runtime_error("Database '" + dbname + "' already exists");
    }
    std::filesystem::create_directory(db_path);
    std::cout << "Database '" + dbname + "' created\n";
}

void DatabaseManager::dropDatabase(const std::string &dbname) {
    std::filesystem::path db_path = "data/" + dbname;
    if (!std::filesystem::exists(db_path)) {
        throw std::runtime_error("Database '" + dbname + "' does not exist");
    }
    std::filesystem::remove_all(db_path);
    if (current_db && current_db->getName() == dbname)
        current_db.reset();
    std::cout << "Database '" + dbname + "' dropped\n";
}

void DatabaseManager::useDatabase(const std::string &dbname) {

    if (current_db && current_db->getName() == dbname) {
        std::cout << "Already using database '" + dbname + "'" << std::endl;
        return;
    }

    std::filesystem::path db_path = "data/" + dbname;

    if (!std::filesystem::exists(db_path)) {
        throw std::runtime_error("Database '" + dbname + "' does not exist");
    }

    current_db = std::make_unique<Database>(dbname);

    std::cout << "Switched to database '" + dbname + "'\n";
}

Database *DatabaseManager::getCurrentDatabase() const {
    return current_db.get();
}