#include "Database.hpp"
#include <stdexcept>
#include <iostream>

Database::Database(const std::string& name) : name(name) {}

Database::~Database() {}

void Database::createTable(const std::string& table_name, const std::vector<Column>& columns) {
    for (const auto& table : tables) {
        if (table->name == table_name) {
            throw std::runtime_error("Table '" + table_name + "' already exists");
        }
    }
    tables.push_back(std::make_unique<Table>(name, table_name, columns));
    std::cout << "Table '" + table_name + "' created\n";
}

void Database::dropTable(const std::string& table_name) {
    auto it = std::remove_if(tables.begin(), tables.end(), [&](const auto& table) {
        return table->name == table_name;
    });
    if (it == tables.end()) {
        throw std::runtime_error("Table '" + table_name + "' does not exist");
    }
    tables.erase(it, tables.end());
    std::cout << "Table '" + table_name + "' dropped\n";
}

Table* Database::getTable(const std::string& table_name) {
    for (auto& table : tables) {
        if (table->name == table_name) return table.get();
    }
    throw std::runtime_error("Table '" + table_name + "' does not exist");
}