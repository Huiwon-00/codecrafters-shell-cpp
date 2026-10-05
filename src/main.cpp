#pragma once
#include "builtin.h"
#include "redirect.h"

namespace fs = std::filesystem;
std::vector<std::string> split(std::string, char);

int main()
{
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string command;
  std::unordered_set<std::string> builtins = {"exit", "echo", "type", "pwd", "cd"};
  std::string filepath(std::getenv("PATH"));

  std::vector<std::string> paths = split(filepath, ':');
  std::string current_directory = std::filesystem::current_path().string();

  while (true)
  {
    std::cout << "$ ";
    if (!std::getline(std::cin, command))
      break;
    std::vector<std::string> arguments = split(command, ' ');
    if (arguments.empty())
      continue;
    auto [redirectType, file] = remove_redirect_sign(arguments);
    int restore_fd = -1;
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
    if(restore_fd!= -1){
      restore(restore_fd, redirectType);
    }
  }
}

std::vector<std::string> split(std::string command, char spliter)
{
  std::vector<int> nums {0, 1, 2};
  int status = 0;
  std::vector<std::string> arguments;
  bool insideSQ = false;
  std::string arg;

  for (size_t i = 0; i < command.length(); i++)
  {
    if(command[i] == '\\'){
      if(status == 0 && i + 1 < command.length()){
        i++;
        arg.push_back(command[i]);
      }else{
        if((command[i + 1] == '\\'|| command[i + 1] == '"') && status == 2){
          i++;
          arg.push_back(command[i]); 
        }
        else{
          arg.push_back(command[i]); 
        }
               
      }
    }
    else if (command[i] == '\'')
    {
      if(status == 1){
        status = 0;
      }
      else if(status == 2){
        arg.push_back(command[i]);
      }
      else{
        status = 1;
      }
    }
    else if(command[i] == '"'){
      if(status == 2){
        status = 0;
      }
      else if(status == 1){
        arg.push_back(command[i]);
      }
      else{
        status = 2;
      }
    }
    else if (status == 1 || status == 2)
    {
      arg.push_back(command[i]);
    }
    else if (command[i] == spliter)
    {
      if (!arg.empty())
      {
        arguments.push_back(arg);
        arg.clear();
      }
    }
    else
    {
      arg.push_back(command[i]);
    }
  }
  if (!arg.empty()) arguments.push_back(arg);

  return arguments; // (Only required if returnType is not void)
}
