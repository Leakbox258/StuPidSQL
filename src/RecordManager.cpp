#include "RecordManager.hpp"
#include <cstring>
#include <stdexcept>

RecordManager::RecordManager(const std::string &filename,
                             const std::vector<Column> &columns)
    : filename(filename), columns(columns) {
    record_size = 1; // Flag byte
    for (const auto &col : columns) {
        record_size += (col.type == ColumnType::INT) ? sizeof(int) : 256;
    }

    if (!openFile_try(std::ios::in | std::ios::out | std::ios::binary)) {
        createFile(std::ios::out | std::ios::binary);
        file.close();
        openFile(std::ios::in | std::ios::out | std::ios::binary);
    }
}

bool RecordManager::openFile_try(std::ios_base::openmode mode) {
    file.open(filename, mode);
    if (!file.is_open()) {
        return false;
    }
    return true;
}

void RecordManager::openFile(std::ios_base::openmode mode) {
    file.open(filename, mode);
    if (!file.is_open())
        throw std::runtime_error("Failed to open file: " + filename);
}

void RecordManager::createFile(std::ios_base::openmode mode) {
    file.open(filename, mode);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    char flag = 0; // Not in use
    for (size_t i = 0; i < record_size; ++i) {
        file.write(&flag, 1);
    }
    file.flush();
}

size_t RecordManager::insertRecord(const std::vector<std::string> &values) {
    file.seekg(0, std::ios::end);
    size_t pos = file.tellg();
    size_t record_id = pos / record_size;
    char flag = 1;
    file.write(&flag, 1);
    for (size_t i = 0; i < columns.size(); ++i) {
        if (columns[i].type == ColumnType::INT) {
            int val = std::stoi(values[i]);
            file.write(reinterpret_cast<const char *>(&val), sizeof(int));
        } else {
            std::string val =
                values[i].substr(1, values[i].size() - 2); // Remove quotes
            char buffer[256] = {0};
            std::strncpy(buffer, val.c_str(), 255);
            file.write(buffer, 256);
        }
    }
    file.flush();
    return record_id;
}

std::vector<std::string> RecordManager::readRecord(size_t record_id) {
    std::vector<std::string> record;
    file.seekg(record_id * record_size);
    char flag;
    file.read(&flag, 1);
    if (flag == 0)
        return record; // Deleted
    for (const auto &col : columns) {
        if (col.type == ColumnType::INT) {
            int val;
            file.read(reinterpret_cast<char *>(&val), sizeof(int));
            record.push_back(std::to_string(val));
        } else {
            char buffer[256];
            file.read(buffer, 256);
            record.push_back(std::string(buffer));
        }
    }
    return record;
}

void RecordManager::deleteRecord(size_t record_id) {
    file.seekp(record_id * record_size);
    char flag = 0;
    file.write(&flag, 1); // write 0 to mark as not in use
    file.flush();
}

void RecordManager::updateRecord(size_t record_id, size_t col_idx,
                                 const std::string &value) {
    file.seekp(record_id * record_size + 1); // Skip flag

    for (size_t i = 0; i < col_idx; ++i) {
        file.seekp((columns[i].type == ColumnType::INT) ? sizeof(int) : 256,
                   std::ios::cur);
    }

    if (columns[col_idx].type == ColumnType::INT) {

        int val = std::stoi(value);
        file.write(reinterpret_cast<const char *>(&val), sizeof(int));

    } else {

        std::string val = value.substr(1, value.size() - 2); // Remove quotes
        char buffer[256] = {0};
        std::strncpy(buffer, val.c_str(), 255);
        file.write(buffer, 256);
    }

    file.flush();
}

size_t RecordManager::getRecordCount() {

    file.seekg(0, std::ios::end);
    return static_cast<size_t>(file.tellg()) / record_size;
}

void RecordManager::clear() {

    file.close();
    openFile(std::ios::out | std::ios::binary | std::ios::trunc);
    file.close();
    openFile(std::ios::in | std::ios::out | std::ios::binary);
}

bool RecordManager::matchCondition(const std::vector<std::string> &record,
                                   size_t col_idx, const std::string &op,
                                   const std::string &value) {
    if (record.empty())
        return false;
    if (columns[col_idx].type == ColumnType::INT) {

        int rec_val = std::stoi(record[col_idx]);
        int cond_val = std::stoi(value);

        if (op == "=" || op == "==")
            return rec_val == cond_val;
        if (op == "<")
            return rec_val < cond_val;
        if (op == ">")
            return rec_val > cond_val;
        if (op == ">=")
            return rec_val >= cond_val;
        if (op == "<=")
            return rec_val <= cond_val;
        if (op == "!=")
            return rec_val != cond_val;

    } else {
        std::string rec_val = record[col_idx];
        std::string cond_val =
            value.substr(1, value.size() - 2); // Remove quotes

        if (op == "=")
            return rec_val == cond_val;
        if (op == "<")
            return rec_val < cond_val;
        if (op == ">")
            return rec_val > cond_val;
        if (op == ">=")
            return rec_val >= cond_val;
        if (op == "<=")
            return rec_val <= cond_val;
        if (op == "!=")
            return rec_val != cond_val;
    }
    return false;
}