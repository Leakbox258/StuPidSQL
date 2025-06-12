#include "Table.hpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

Table::Table(const std::string &db_name, const std::string &table_name,
             const std::vector<Column> &columns, bool isNew)
    : db_name(db_name), name(table_name), columns(columns),
      record_manager(pwd + db_name + "/" + table_name + ".dat", columns) {
    for (const auto &col : columns) {
        if (col.is_primary) {
            index_manager = std::make_unique<IndexManager>(
                pwd + db_name + "/" + table_name + ".idx", col);
            break;
        }
    }
}

size_t Table::getColumnIndex(const std::string &column_name) const {
    for (size_t i = 0; i < columns.size(); ++i) {
        if (columns[i].name == column_name)
            return i;
    }
    throw std::runtime_error("Column '" + column_name + "' not found");
}

void Table::insert(const std::vector<std::string> &values) {
    if (values.size() != columns.size()) {
        throw std::runtime_error("Value count does not match column count");
    }
    size_t record_id = record_manager.insertRecord(values);
    if (index_manager) {
        size_t key_idx = getColumnIndex(index_manager->getKeyColumn().name);
        index_manager->insertKey(values[key_idx], record_id);
    }
    std::cout << "1 row inserted\n";
}

void Table::select(const std::string &column, const std::string &where_cond) {
    bool select_all = (column == "*");
    size_t col_idx = select_all ? 0 : getColumnIndex(column);
    if (!where_cond.empty()) {
        std::istringstream iss(where_cond);
        std::string col_name, op, val;
        iss >> col_name >> op >> val;
        size_t cond_idx = getColumnIndex(col_name);
        if (index_manager && columns[cond_idx].is_primary) {
            auto record_ids = index_manager->find(val, op);
            for (size_t id : record_ids) {
                auto record = record_manager.readRecord(id);
                if (select_all) {
                    for (const auto &v : record)
                        std::cout << v << " ";
                    std::cout << "\n";
                } else {
                    std::cout << record[col_idx] << "\n";
                }
            }
        } else {
            for (size_t i = 0; i < record_manager.getRecordCount(); ++i) {
                auto record = record_manager.readRecord(i);
                if (record_manager.matchCondition(record, cond_idx, op, val)) {
                    if (select_all) {
                        for (const auto &v : record)
                            std::cout << v << " ";
                        std::cout << "\n";
                    } else {
                        std::cout << record[col_idx] << "\n";
                    }
                }
            }
        }
    } else {
        for (size_t i = 0; i < record_manager.getRecordCount(); ++i) {
            auto record = record_manager.readRecord(i);
            if (select_all) {
                for (const auto &v : record)
                    std::cout << v << " ";
                std::cout << "\n";
            } else {
                std::cout << record[col_idx] << "\n";
            }
        }
    }
}

void Table::del(const std::string &where_cond) {
    size_t deleted = 0;
    if (!where_cond.empty()) {
        std::istringstream iss(where_cond);
        std::string col_name, op, val;
        iss >> col_name >> op >> val;
        size_t cond_idx = getColumnIndex(col_name);
        if (index_manager && columns[cond_idx].is_primary) {
            auto record_ids = index_manager->find(val, op);
            for (size_t id : record_ids) {
                record_manager.deleteRecord(id);
                index_manager->removeKey(val);
                ++deleted;
            }
        } else {
            for (size_t i = 0; i < record_manager.getRecordCount(); ++i) {
                auto record = record_manager.readRecord(i);
                if (record_manager.matchCondition(record, cond_idx, op, val)) {
                    record_manager.deleteRecord(i);
                    if (index_manager)
                        index_manager->removeKey(record[getColumnIndex(
                            index_manager->getKeyColumn().name)]);
                    ++deleted;
                }
            }
        }
    } else {
        deleted = record_manager.getRecordCount();
        record_manager.clear();
        if (index_manager)
            index_manager->clear();
    }
    std::cout << deleted << " row(s) deleted\n";
}

void Table::update(const std::string &column, const std::string &value,
                   const std::string &where_cond) {
    size_t col_idx = getColumnIndex(column);
    if (columns[col_idx].is_primary) {
        throw std::runtime_error("Cannot update primary key");
    }
    size_t updated = 0;
    if (!where_cond.empty()) {
        std::istringstream iss(where_cond);
        std::string cond_col, op, cond_val;
        iss >> cond_col >> op >> cond_val;
        size_t cond_idx = getColumnIndex(cond_col);
        if (index_manager && columns[cond_idx].is_primary) {
            auto record_ids = index_manager->find(cond_val, op);
            for (size_t id : record_ids) {
                record_manager.updateRecord(id, col_idx, value);
                ++updated;
            }
        } else {
            for (size_t i = 0; i < record_manager.getRecordCount(); ++i) {
                auto record = record_manager.readRecord(i);
                if (record_manager.matchCondition(record, cond_idx, op,
                                                  cond_val)) {
                    record_manager.updateRecord(i, col_idx, value);
                    ++updated;
                }
            }
        }
    } else {
        for (size_t i = 0; i < record_manager.getRecordCount(); ++i) {
            record_manager.updateRecord(i, col_idx, value);
            ++updated;
        }
    }
    std::cout << updated << " row(s) updated\n";
}