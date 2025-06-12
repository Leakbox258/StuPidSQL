#include "IndexManager.hpp"
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <vector>

IndexManager::IndexManager(const std::string &filename,
                           const Column &key_column)
    : filename(filename), key_column(key_column) {

    std::ifstream inFile(filename, std::ios::binary);

    if (inFile) {
        // file header
        uint32_t magic;
        uint8_t file_col_type;
        uint32_t record_count;

        inFile.read(reinterpret_cast<char *>(&magic), sizeof(magic));
        inFile.read(reinterpret_cast<char *>(&file_col_type),
                    sizeof(file_col_type));
        inFile.read(reinterpret_cast<char *>(&record_count),
                    sizeof(record_count));

        // verify magic number
        if (magic != INDEX_MAGIC) {
            throw std::runtime_error("Invalid index file format: " + filename);
        }

        // verify value type
        uint8_t expected_type = (key_column.type == ColumnType::INT) ? 0 : 1;
        if (file_col_type != expected_type) {
            throw std::runtime_error("Column type mismatch in index file: " +
                                     filename);
        }

        // verify record count
        for (uint32_t i = 0; i < record_count; ++i) {
            if (key_column.type == ColumnType::INT) {
                int key;
                uint32_t record_id;
                inFile.read(reinterpret_cast<char *>(&key), sizeof(key));
                inFile.read(reinterpret_cast<char *>(&record_id),
                            sizeof(record_id));
                index[std::to_string(key)] = record_id;
            } else {
                char key_buffer[256];
                uint32_t record_id;
                inFile.read(key_buffer, sizeof(key_buffer));
                inFile.read(reinterpret_cast<char *>(&record_id),
                            sizeof(record_id));
                index[std::string(key_buffer)] = record_id;
            }
        }
    } else {
        flush();
    }
}

void IndexManager::insertKey(const std::string &key, size_t record_id) {
    index[key] = record_id;
}

void IndexManager::removeKey(const std::string &key) { index.erase(key); }

std::vector<size_t> IndexManager::find(const std::string &key,
                                       const std::string &op) const {
    std::vector<size_t> results;
    if (key_column.type == ColumnType::INT) {
        int val = std::stoi(key);
        for (const auto &[k, id] : index) {
            int k_val = std::stoi(k);
            if ((op == "=" && k_val == val) || (op == "<" && k_val < val) ||
                (op == ">" && k_val > val)) {
                results.push_back(id);
            }
        }
    } else {
        std::string val = key.substr(1, key.size() - 2); // Remove quotes
        for (const auto &[k, id] : index) {
            if ((op == "=" && k == val) || (op == "<" && k < val) ||
                (op == ">" && k > val)) {
                results.push_back(id);
            }
        }
    }
    return results;
}

void IndexManager::clear() { index.clear(); }

void IndexManager::flush() {
    std::ofstream file(filename, std::ios::binary | std::ios::trunc);

    if (!file) {
        throw std::runtime_error("Failed to create index file: " + filename);
    }

    // file head
    uint32_t magic = INDEX_MAGIC;
    uint8_t col_type = (key_column.type == ColumnType::INT) ? 0 : 1;
    uint32_t record_count = static_cast<uint32_t>(index.size());

    file.write(reinterpret_cast<const char *>(&magic), sizeof(magic));
    file.write(reinterpret_cast<const char *>(&col_type), sizeof(col_type));
    file.write(reinterpret_cast<const char *>(&record_count),
               sizeof(record_count));

    for (const auto &[key, record_id] : index) {
        uint32_t id = static_cast<uint32_t>(record_id);

        if (key_column.type == ColumnType::INT) {
            int int_key = std::stoi(key);
            file.write(reinterpret_cast<const char *>(&int_key),
                       sizeof(int_key));
        } else {
            char str_key[256] = {0};
            std::strncpy(str_key, key.c_str(), sizeof(str_key) - 1);
            file.write(str_key, sizeof(str_key));
        }

        file.write(reinterpret_cast<const char *>(&id), sizeof(id));
    }
}