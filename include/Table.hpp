#pragma once
#ifndef TABLE_HPP
#define TABLE_HPP

#include "Column.hpp"
#include "IndexManager.hpp"
#include "RecordManager.hpp"
#include <memory>
#include <string>
#include <vector>

class Table {
public:
    Table(const std::string &db_name, const std::string &table_name,
          const std::vector<Column> &columns, bool isNew = true);
    void insert(const std::vector<std::string> &values);
    void select(const std::string &column, const std::string &where_cond = "");
    void del(const std::string &where_cond = "");
    void update(const std::string &column, const std::string &value,
                const std::string &where_cond = "");

    std::string name;
    std::vector<Column> columns;

private:
    std::string db_name;
    RecordManager record_manager;
    std::unique_ptr<IndexManager> index_manager;
    size_t getColumnIndex(const std::string &column_name) const;
};

#endif