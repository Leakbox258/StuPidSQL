#include "Executor.hpp"
#include "Database.hpp"
#include <stdexcept>

Executor::Executor(DatabaseManager &db_manager) : db_manager(db_manager) {}

void Executor::execute(const Command &cmd) {
    /// DDL

    if (auto *create_db = dynamic_cast<const CreateDatabaseCommand *>(&cmd)) {

        db_manager.createDatabase(create_db->dbname);

    } else if (auto *drop_db =
                   dynamic_cast<const DropDatabaseCommand *>(&cmd)) {

        db_manager.dropDatabase(drop_db->dbname);

    } else if (auto *use_db = dynamic_cast<const UseDatabaseCommand *>(&cmd)) {

        db_manager.useDatabase(use_db->dbname);

    } else if (auto *create_tbl =
                   dynamic_cast<const CreateTableCommand *>(&cmd)) {

        if (!db_manager.getCurrentDatabase())
            throw std::runtime_error("No database selected");

        db_manager.getCurrentDatabase()->createTable(create_tbl->table_name,
                                                     create_tbl->columns);

    } else if (auto *drop_tbl = dynamic_cast<const DropTableCommand *>(&cmd)) {

        if (!db_manager.getCurrentDatabase())
            throw std::runtime_error("No database selected");

        db_manager.getCurrentDatabase()->dropTable(drop_tbl->table_name);

    }

    /// DML

    else if (auto *insert = dynamic_cast<const InsertCommand *>(&cmd)) {

        if (!db_manager.getCurrentDatabase())
            throw std::runtime_error("No database selected");

        db_manager.getCurrentDatabase()
            ->getTable(insert->table_name)
            ->insert(insert->values);

    } else if (auto *select = dynamic_cast<const SelectCommand *>(&cmd)) {

        if (!db_manager.getCurrentDatabase())
            throw std::runtime_error("No database selected");

        db_manager.getCurrentDatabase()
            ->getTable(select->table_name)
            ->select(select->column, select->where_cond);

    } else if (auto *del = dynamic_cast<const DeleteCommand *>(&cmd)) {

        if (!db_manager.getCurrentDatabase())
            throw std::runtime_error("No database selected");

        db_manager.getCurrentDatabase()
            ->getTable(del->table_name)
            ->del(del->where_cond);

    } else if (auto *update = dynamic_cast<const UpdateCommand *>(&cmd)) {

        if (!db_manager.getCurrentDatabase())
            throw std::runtime_error("No database selected");

        db_manager.getCurrentDatabase()
            ->getTable(update->table_name)
            ->update(update->column, update->value, update->where_cond);

    } else {

        throw std::runtime_error("Unsupported command");
    }
}