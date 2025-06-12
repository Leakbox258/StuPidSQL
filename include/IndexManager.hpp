#pragma once
#ifndef INDEX_MANAGER_HPP
#define INDEX_MANAGER_HPP

#include "Column.hpp"
#include <map>
#include <string>
#include <vector>

class IndexManager {
public:
    IndexManager(const std::string &filename, const Column &key_column);
    ~IndexManager();
    void insertKey(const std::string &key, size_t record_id);
    void removeKey(const std::string &key);
    std::vector<size_t> find(const std::string &key,
                             const std::string &op) const;
    void clear();
    void flush();
    Column getKeyColumn() const { return key_column; }

private:
    std::string filename;
    Column key_column;
    std::map<std::string, size_t> index;
    static constexpr uint32_t INDEX_MAGIC = 0x11451519;
};

#endif