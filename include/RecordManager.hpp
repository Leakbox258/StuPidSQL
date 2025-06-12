#pragma once
#ifndef RECORD_MANAGER_HPP
#define RECORD_MANAGER_HPP

#include "Column.hpp"
#include <fstream>
#include <string>
#include <vector>

class RecordManager {
public:
    RecordManager(const std::string &filename, const std::vector<Column> &columns);
    size_t insertRecord(const std::vector<std::string> &values);
    std::vector<std::string> readRecord(size_t record_id);
    void deleteRecord(size_t record_id);
    void updateRecord(size_t record_id, size_t col_idx, const std::string &value);
    size_t getRecordCount();
    void clear();
    bool matchCondition(const std::vector<std::string> &record, size_t col_idx, const std::string &op,
                        const std::string &value);

private:
    std::string filename;
    std::vector<Column> columns;
    size_t record_size;
    std::fstream file;
    void openFile(std::ios_base::openmode mode);
};

#endif