#include "builtin.h"
#include "redirect.h"
#include "pipeline.h"
#include "tokenizer.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string command;
  std::unordered_set<std::string> builtins = {"exit", "echo", "type", "pwd", "cd"};
  const char* path_env {std::getenv("PATH")};
  std::string filepath{path_env ? path_env : ""};

  std::vector<std::string> paths = tokenize(filepath, ':');
  std::string current_directory = std::filesystem::current_path().string();

  while (true)
  {
    std::cout << "$ ";
    if (!std::getline(std::cin, command))
      break;
    std::vector<std::string> arguments = tokenize(command, ' ');
    if (arguments.empty())
      continue;
    if(check_pipeline(arguments)){
      pipeline(arguments, paths, builtins, current_directory);
    }
    else{
      auto [redirectType, file] = remove_redirect_sign(arguments);
      int restore_fd = -2;
      if(!file.empty()){
        if(builtins.contains(arguments[0])){
          if(redirectType == 1){
            restore_fd= R_stdout(file);
          }
          else if(redirectType == 2){
            restore_fd = R_stderr(file);
          }
          else if(redirectType == 3){
            restore_fd = R_append(file);
          }
          else if(redirectType == 4){
            restore_fd = R_append_stderr(file);
          }
        }
      }
      if(restore_fd == -1){
        continue;
      }
      if (arguments[0] == "exit")
        break;
      else if (arguments[0] == "echo")
        echo(arguments);
      else if (arguments[0] == "pwd")
        pwd(current_directory);
      else if (arguments[0] == "cd")
        cd(arguments, current_directory);
      else if (arguments[0] == "type")
        type(arguments, paths, builtins);
      else
        executable(redirectType, arguments, paths, file);
      if(restore_fd!= -2){
        restore(restore_fd, redirectType);
      }
    }
  }
}

