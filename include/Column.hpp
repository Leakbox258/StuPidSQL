#pragma once
#ifndef COLUMN_HPP
#define COLUMN_HPP

#include <string>

enum class ColumnType { INT, STRING };

struct Column {
    std::string name;
    ColumnType type;
    bool is_primary = false;
};

#endif