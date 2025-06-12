#pragma once
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Column.hpp"
#include <string>
#include <vector>

struct Command {
    virtual ~Command() = default;
};

struct CreateDatabaseCommand : Command {
    std::string dbname;
    explicit CreateDatabaseCommand(const std::string &dbname) : dbname(dbname) {}
};

struct DropDatabaseCommand : Command {
    std::string dbname;
    explicit DropDatabaseCommand(const std::string &dbname) : dbname(dbname) {}
};

struct UseDatabaseCommand : Command {
    std::string dbname;
    explicit UseDatabaseCommand(const std::string &dbname) : dbname(dbname) {}
};

struct CreateTableCommand : Command {
    std::string table_name;
    std::vector<Column> columns;
    CreateTableCommand(const std::string &table_name, const std::vector<Column> &columns)
        : table_name(table_name), columns(columns) {}
};

struct DropTableCommand : Command {
    std::string table_name;
    explicit DropTableCommand(const std::string &table_name) : table_name(table_name) {}
};

struct InsertCommand : Command {
    std::string table_name;
    std::vector<std::string> values;
    InsertCommand(const std::string &table_name, const std::vector<std::string> &values)
        : table_name(table_name), values(values) {}
};

struct SelectCommand : Command {
    std::string table_name;
    std::string column;
    std::string where_cond;
    SelectCommand(const std::string &table_name, const std::string &column, const std::string &where_cond = "")
        : table_name(table_name), column(column), where_cond(where_cond) {}
};

struct DeleteCommand : Command {
    std::string table_name;
    std::string where_cond;
    DeleteCommand(const std::string &table_name, const std::string &where_cond = "")
        : table_name(table_name), where_cond(where_cond) {}
};

struct UpdateCommand : Command {
    std::string table_name;
    std::string column;
    std::string value;
    std::string where_cond;
    UpdateCommand(const std::string &table_name, const std::string &column, const std::string &value,
                  const std::string &where_cond = "")
        : table_name(table_name), column(column), value(value), where_cond(where_cond) {}
};

#endif