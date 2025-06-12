#include "IndexManager.hpp"
#include <stdexcept>
#include <vector>

IndexManager::IndexManager(const std::string &filename, const Column &key_column)
    : filename(filename), key_column(key_column) {}

void IndexManager::insertKey(const std::string &key, size_t record_id) { index[key] = record_id; }

void IndexManager::removeKey(const std::string &key) { index.erase(key); }

std::vector<size_t> IndexManager::find(const std::string &key, const std::string &op) const {
    std::vector<size_t> results;
    if (key_column.type == ColumnType::INT) {
        int val = std::stoi(key);
        for (const auto &[k, id] : index) {
            int k_val = std::stoi(k);
            if ((op == "=" && k_val == val) || (op == "<" && k_val < val) || (op == ">" && k_val > val)) {
                results.push_back(id);
            }
        }
    } else {
        std::string val = key.substr(1, key.size() - 2); // Remove quotes
        for (const auto &[k, id] : index) {
            if ((op == "=" && k == val) || (op == "<" && k < val) || (op == ">" && k > val)) {
                results.push_back(id);
            }
        }
    }
    return results;
}

void IndexManager::clear() { index.clear(); }