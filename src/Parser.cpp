#include "Parser.hpp"
#include <sstream>
#include <stdexcept>

std::vector<std::string> Parser::tokenize(const std::string &sql) {
    std::vector<std::string> tokens;
    std::string token;
    bool in_quotes = false;
    for (char c : sql) {
        if (c == '"') {
            in_quotes = !in_quotes;
            token += c;
        } else if (c == ' ' && !in_quotes) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else {
            token += c;
        }
    }
    if (!token.empty())
        tokens.push_back(token);
    return tokens;
}

std::unique_ptr<Command> Parser::parse(const std::string &sql) {
    auto tokens = tokenize(sql);

    if (tokens.empty())

        throw std::runtime_error("Empty command");

    if (tokens[0] == "create" && tokens[1] == "database") {

        if (tokens.size() < 3)
            throw std::runtime_error("Missing database name");

        return std::make_unique<CreateDatabaseCommand>(tokens[2]);
    }
    if (tokens[0] == "drop" && tokens[1] == "database") {

        if (tokens.size() < 3)
            throw std::runtime_error("Missing database name");

        return std::make_unique<DropDatabaseCommand>(tokens[2]);
    }
    if (tokens[0] == "use") {

        if (tokens.size() < 2)
            throw std::runtime_error("Missing database name");

        return std::make_unique<UseDatabaseCommand>(tokens[1]);
    }
    if (tokens[0] == "create" && tokens[1] == "table") {

        if (tokens.size() < 5)
            throw std::runtime_error("Invalid create table syntax");

        std::string table_name = tokens[2];
        std::vector<Column> columns;
        std::string defs = sql.substr(sql.find('(') + 1);
        defs = defs.substr(0, defs.find_last_of(')'));
        std::istringstream iss(defs);
        std::string col_def;

        while (std::getline(iss, col_def, ',')) {
            std::istringstream col_ss(col_def);
            std::string name, type, primary;
            col_ss >> name >> type >> primary;
            Column col{name,
                       (type == "int" ? ColumnType::INT : ColumnType::STRING)};
            if (primary == "primary")
                col.is_primary = true;
            columns.push_back(col);
        }

        return std::make_unique<CreateTableCommand>(table_name, columns);
    }

    if (tokens[0] == "drop" && tokens[1] == "table") {
        if (tokens.size() < 3)
            throw std::runtime_error("Missing table name");

        return std::make_unique<DropTableCommand>(tokens[2]);
    }

    if (tokens[0] == "insert") {
        if (tokens.size() < 4 || tokens[2] != "values")
            throw std::runtime_error("Invalid insert syntax");

        std::string table_name = tokens[1];
        std::string vals = sql.substr(sql.find('(') + 1);
        vals = vals.substr(0, vals.find_last_of(')'));
        std::vector<std::string> values;
        std::istringstream iss(vals);
        std::string val;
        while (std::getline(iss, val, ',')) {
            values.push_back(val);
        }

        return std::make_unique<InsertCommand>(table_name, values);
    }

    if (tokens[0] == "select") {
        if (tokens.size() < 4 || tokens[2] != "from")
            throw std::runtime_error("Invalid select syntax");

        std::string column = tokens[1];
        std::string table_name = tokens[3];
        std::string where_cond;
        if (tokens.size() > 4 && tokens[4] == "where") {
            where_cond = sql.substr(sql.find("where") + 6);
        }

        return std::make_unique<SelectCommand>(table_name, column, where_cond);
    }

    if (tokens[0] == "delete") {

        if (tokens.size() < 2)
            throw std::runtime_error("Invalid delete syntax");

        std::string table_name = tokens[1];
        std::string where_cond;

        if (tokens.size() > 2 && tokens[2] == "where") {
            where_cond = sql.substr(sql.find("where") + 6);
        }

        return std::make_unique<DeleteCommand>(table_name, where_cond);
    }

    if (tokens[0] == "update") {

        if (tokens.size() < 5 || tokens[2] != "set")
            throw std::runtime_error("Invalid update syntax");

        std::string table_name = tokens[1];
        std::string set_clause = sql.substr(sql.find("set") + 4);
        std::istringstream iss(set_clause);
        std::string column, eq, value;
        iss >> column >> eq >> value;
        std::string where_cond;

        if (sql.find("where") != std::string::npos) {
            where_cond = sql.substr(sql.find("where") + 6);
        }

        return std::make_unique<UpdateCommand>(table_name, column, value,
                                               where_cond);
    }

    throw std::runtime_error("Unknown command");
}