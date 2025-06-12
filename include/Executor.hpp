#pragma once
#ifndef EXECUTOR_HPP
#define EXECUTOR_HPP

#include "Command.hpp"
#include "DatabaseManager.hpp"

class Executor {
public:
    explicit Executor(DatabaseManager &db_manager);
    void execute(const Command &cmd);

private:
    DatabaseManager &db_manager;
};

#endif