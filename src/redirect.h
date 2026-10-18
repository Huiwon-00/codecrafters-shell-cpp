#pragma once
#include <string>
#include <vector>
#include <utility>

int R_stdout(const std::string& file);
int R_stderr(const std::string& file);
int R_append(const std::string& file);
int R_append_stderr(const std::string& file);
void restore(int restore_fd, int redirectType);
std::pair<int, std::string> remove_redirect_sign(std::vector<std::string>& arguments);
