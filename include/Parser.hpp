#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include "Command.hpp"
#include <memory>
#include <string>

class Parser {
public:
    static std::unique_ptr<Command> parse(const std::string &sql);

private:
    static std::vector<std::string> tokenize(const std::string &sql);
};

#endif