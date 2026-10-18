#pragma once
#include "builtin.h"

bool check_pipeline(const std::vector<std::string>& arguments);
void pipeline(const std::vector<std::string>& arguments, const std::vector<std::string>& paths,
    const std::unordered_set<std::string>& builtins, std::string& current_directory);


