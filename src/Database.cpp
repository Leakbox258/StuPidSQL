#include "Database.hpp"
#include "tools.hpp"
#include <iostream>
#include <map>
#include <stdexcept>

Database::Database(const std::string &name) : name(name) {

    struct info {
        uint8_t ready = 0;
        std::vector<Column> columns{};
    };

    std::map<std::string, info> loadtb{};

    for (auto &file : std::filesystem::directory_iterator(pwd + name)) {

        if (!file.is_regular_file())
            continue;

        auto filename = file.path().string();

        auto last_slash = filename.find_last_of('/');
        auto tb_name = filename.substr(last_slash + 1,
                                       filename.find_first_of('.', last_slash) -
                                           (last_slash + 1));

        ++loadtb[tb_name].ready;

        if (filename.contains(".info")) {
            std::fstream file(filename, std::ios::binary | std::ios::in);

            if (!file)
                throw std::runtime_error("fail to open " + filename);

            file.seekg(0, std::ios::end);

            const auto size = file.tellg();

            file.seekg(0, std::ios::beg);

            std::string content;
            content.resize(size);

            file.read(&content[0], size);

            auto &columns = loadtb[tb_name].columns;
            for (int i = 0; i < size;) {
                // a new column info
                /* type, primary, name  */
                Column column;
                content[i++] == '\x00' ? column.type = ColumnType::INT
                                       : column.type = ColumnType::STRING;
                content[i++] == '\x00' ? void(column.is_primary = true) : nop;

                auto end_pos = content.find_first_of('\xff', i);
                column.name = content.substr(i, end_pos - i);
                i = end_pos + 1;

                columns.emplace_back(column);
            }
        }
    }

    for (auto &[tb_name, info] : loadtb) {
        if (info.ready != 3)
            continue;

        tables.emplace_back(std::make_unique<Table>(
            name, tb_name, info.columns)); // name = db_name
    }
}

Database::~Database() {}

void Database::createTable(const std::string &table_name,
                           const std::vector<Column> &columns) {
    for (const auto &table : tables) {
        if (table->name == table_name) {
            throw std::runtime_error("Table '" + table_name +
                                     "' already exists");
        }
    }
    tables.push_back(std::make_unique<Table>(name, table_name, columns));
    std::cout << "Table '" + table_name + "' created\n";
}

void Database::dropTable(const std::string &table_name) {
    auto it =
        std::remove_if(tables.begin(), tables.end(), [&](const auto &table) {
            return table->name == table_name;
        });

    if (it == tables.end()) {
        throw std::runtime_error("Table '" + table_name + "' does not exist");
    }

    tables.erase(it, tables.end());

    for (auto &file : std::filesystem::directory_iterator(pwd + name)) {
        auto filename = file.path().string();

        if (inSet(filename, // NOLINT
                  pwd + name + '/' + table_name + ".idx",
                  pwd + name + '/' + table_name + ".info",
                  pwd + name + '/' + table_name + ".dat")) {

            std::filesystem::remove(file.path());
        }
    }

    std::cout << "Table '" + table_name + "' dropped\n";
}

Table *Database::getTable(const std::string &table_name) {
    for (auto &table : tables) {

        if (table->name == table_name)
            return table.get();
    }

    throw std::runtime_error("Table '" + table_name + "' does not exist");
}