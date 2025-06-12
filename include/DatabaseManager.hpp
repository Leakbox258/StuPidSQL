#pragma once
#ifndef DATABASE_MANAGER_HPP
#define DATABASE_MANAGER_HPP

#include <memory>
#include <string>

class Database;

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();
    void createDatabase(const std::string &dbname);
    void dropDatabase(const std::string &dbname);
    void useDatabase(const std::string &dbname);
    Database *getCurrentDatabase() const;

private:
    std::unique_ptr<Database> current_db;
};

#endif