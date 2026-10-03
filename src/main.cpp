#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

namespace fs = std::filesystem;
std::vector<std::string> split(std::string, char);


int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::string filepath(std::getenv("PATH"));
  // std::cout << filepath << "\n";

  std::string current_directory = std::filesystem::current_path().string();
  std::vector<std::string> paths = split(filepath, ':');

  while(true){
    std::cout << "$ ";
    std::string command;
    std::unordered_set<std::string> types = {"exit", "echo", "type", "pwd", "cd"};
    //std::getline(std::cin, command);
    if(!std::getline(std::cin, command)) break;
    std::vector<std::string> arguments = split(command, ' ');

    if(arguments.empty()){
      continue;
    }
    if( arguments[0] == "exit"){
      break;
    }
    else if(arguments[0]  == "echo"){
      if(arguments.size() > 1) {
        for(size_t i = 1; i < arguments.size(); i++){
          if(i != arguments.size() - 1){
              std::cout << arguments[i] << " ";
          }
          else{
            std::cout << arguments[i] << std::endl;
          }
        }
      }
      else{
        std::cout << std::endl;
        continue;
      }
    }
    else if(arguments[0]  == "pwd"){
      std::cout << current_directory <<std::endl;
    }
    else if(arguments[0]  == "cd"){
      if(arguments.size() == 1 || arguments[1] == "~"){
        std::string HOME(std::getenv("HOME"));
        current_directory = HOME;
        std::filesystem::current_path(HOME);
      }
     else if( std::filesystem::is_directory(arguments[1])){
        current_directory = std::filesystem::canonical(arguments[1]).string();
        std::filesystem::current_path(current_directory);
      }
      else{
        std::cout << "cd: " << arguments[1] << ": No such file or directory" << std::endl;
      }
    }
    else if(arguments[0]  == "type"){
      bool typeDone = false;
      if(arguments.size() == 1){
        continue;
      }
      if(types.contains(arguments[1])){
        std::cout << arguments[1] << " is a shell builtin" << std::endl;
        typeDone = true;
      }
      else if( !typeDone ){
        std::string fullpath ;
        for (std::string p : paths){ 
          fullpath = p + "/" + arguments[1];
          //std::cout << fullpath << std::endl;
          if (std::filesystem::is_regular_file(fullpath) && access (fullpath.c_str(), X_OK) == 0){
            std::cout << arguments[1] << " is " << fullpath << std::endl;
            typeDone = true;
            break;
          } 
        } 
      }
      if(!typeDone){
        std::cout << arguments[1] << ": not found" << std::endl;
      }
    }
    else{ // check if the command is executable
      std::string exCommand = arguments[0];
      std::string fullpath;
      bool exCommandDone = false;
      for (std::string p : paths){ 
        fullpath = p + "/" + exCommand;
        //std::cout << fullpath << std::endl;
        if (std::filesystem::is_regular_file(fullpath) && access (fullpath.c_str(), X_OK) == 0){
          exCommandDone = true;
          break;
        } 
      } 
      
      if(exCommandDone){
        std::vector<char*> arguments_c;
        for(std::string& s: arguments){
          arguments_c.push_back(s.data());
        }
        arguments_c.push_back(nullptr);
        pid_t pid = fork();
        if(pid == 0){
          execv(fullpath.c_str(), arguments_c.data());
          std::exit(1);
        }
        else{
          waitpid(pid, nullptr, 0);
        }
      }
      else{
        std::cout << arguments[0] << ": command not found" << std::endl;
      }
    }
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
