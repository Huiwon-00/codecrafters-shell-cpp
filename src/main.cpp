#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

namespace fs = std::filesystem;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::vector<std::string> arguments;
  std::string filepath(std::getenv("PATH"));
  // std::cout << filepath << "\n";


  std::vector<std::string> paths;
  int j {0};
  for(int i = 0; i < filepath.length(); i++) {
    if(filepath[i] == ':' ) {
      paths.push_back(filepath.substr(j, i - j));
      j = i + 1;
    }
    else if(i == filepath.length() - 1 ) {
      paths.push_back(filepath.substr(j));
    }
  }


  // TODO: Uncomment the code below to pass the first stage 
  while(true){
    std::cout << "$ ";
    std::string command;
    std::string current_directory = std::filesystem::current_path().string();
    std::unordered_set<std::string> types = {"exit", "echo", "type", "pwd", "cd"};
    std::getline(std::cin, command);
    std::string keyword;
    for(int i = 0; i < command.length(); i++){
      if(command[i] == ' '){
        break;
      }
      else{
        keyword += command[i];
      }
    }
    if( command == "exit"){
      std::cout << command << std::endl;
      break;
    }
    else if(keyword == "echo"){
      std::cout << command.substr(5) << std::endl;
    }
    else if(keyword == "pwd"){
      std::cout << current_directory <<std::endl;
    }
    else if(keyword == "cd"){
      if( std::filesystem::exists(command.substr(3))){
        std::filesystem::current_path(command.substr(3));
      }
      else{
        std::cout << "cd: " << command.substr(3) << ": No such file or directory" << std::endl;
      }
    }
    else if(keyword == "type"){
      bool typeDone = false;
      if(types.contains(command.substr(5))){
        std::cout << command.substr(5) << " is a shell builtin" << std::endl;
        typeDone = true;
      }
      else if( !typeDone ){
        std::string fullpath ;
        for (std::string p : paths){ 
          fullpath = p + "/" + command.substr(5);
          //std::cout << fullpath << std::endl;
          if (std::filesystem::is_regular_file(fullpath) && access (fullpath.c_str(), X_OK) == 0){
            std::cout << command.substr(5) << " is " << fullpath << std::endl;
            typeDone = true;
            break;
          } 
        } 
      }
      if(!typeDone){
        std::cout << command.substr(5) << ": not found" << std::endl;
      }
    }
    else{ // check if the command is executable
      std::string exCommand = "";
      for(char c : command){
        if( c == ' '){
          break;
        }
        exCommand += c;
      }

      std::string fullpath;
      bool exCommandDone = false;
      for (std::string p : paths){ 
        fullpath = p + "/" + exCommand;
        //std::cout << fullpath << std::endl;
        if (std::filesystem::is_regular_file(fullpath) && access (fullpath.c_str(), X_OK) == 0){
          exCommandDone = true;
          int j {0};
          for(int i = 0; i < command.length(); i++) {
            if(command[i] == ' ' ) {
              arguments.push_back(command.substr(j, i - j));
              j = i + 1;
            }
            else if(i == command.length() - 1 ) {
              arguments.push_back(command.substr(j));
            }
          }
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
      arguments.clear();
      if(!exCommandDone){
        std::cout << command << ": command not found" << std::endl;
      }
    }
  }

  
}



/*
std::cout << "Program was passed " << arguments.size() << ".\n";
        for(int i = 0; i < arguments.size(); i++){
          std::cout << "Arg #" << i << ": " << arguments[i] << std::endl;
        } */