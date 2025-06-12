#pragma once
#ifndef DATABASE_HPP
#define DATABASE_HPP

#include "Table.hpp"
#include <memory>
#include <string>
#include <vector>

class Database {
public:
    explicit Database(const std::string &name);
    ~Database();
    void createTable(const std::string &table_name, const std::vector<Column> &columns);
    void dropTable(const std::string &table_name);
    Table *getTable(const std::string &table_name);
    std::string getName() const { return name; }

private:
    std::string name;
    std::vector<std::unique_ptr<Table>> tables;
};

#endif