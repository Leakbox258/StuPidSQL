#pragma once
#ifndef COLUMN_HPP
#define COLUMN_HPP

#include <filesystem>
#include <string>

inline std::string pwd{};

enum class ColumnType { INT, STRING };

struct Column {
    std::string name;
    ColumnType type;
    bool is_primary = false;
};

#endif