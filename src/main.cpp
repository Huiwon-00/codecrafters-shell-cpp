#include "builtin.h"


namespace fs = std::filesystem;
std::vector<std::string> split(std::string, char);


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string command;
  std::unordered_set<std::string> types = {"exit", "echo", "type", "pwd", "cd"};
  std::string filepath(std::getenv("PATH"));
  
  std::vector<std::string> paths = split(filepath, ':');
  std::string current_directory = std::filesystem::current_path().string();

  while(true){
    std::cout << "$ ";
    if(!std::getline(std::cin, command)) break;
    std::vector<std::string> arguments = split(command, ' ');
    if(arguments.empty()) continue;
    if( arguments[0] == "exit") break;
    else if(arguments[0]  == "echo") echo(arguments);
    else if(arguments[0]  == "pwd") pwd(current_directory);
    else if(arguments[0]  == "cd") cd(arguments, current_directory);
    else if(arguments[0]  == "type") type(arguments, paths, types);
    else executable(arguments, paths);
  }
}

std::vector<std::string> split(std::string command, char spliter) {
    std::vector<std::string> arguments;
    size_t j {0};
    for(size_t i = 0; i < command.length(); i++) {
      if(command[i] == spliter) {
        if(i > j){
          arguments.push_back(command.substr(j, i - j));
        }
          j = i + 1;
      }
      else if(i == command.length() - 1 ) {
        arguments.push_back(command.substr(j));
      }
    }

    return arguments; // (Only required if returnType is not void)
}
