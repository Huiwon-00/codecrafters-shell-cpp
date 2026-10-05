#pragma once
#include "redirect.h"

void echo(std::vector<std::string> arguments){
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
        return;
      }
}

void pwd(std::string current_directory){
    std::cout << current_directory << std::endl;
}

void cd(std::vector<std::string> arguments, std::string& current_directory){
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

void type(std::vector<std::string> arguments, std::vector<std::string> paths, std::unordered_set<std::string> builtins){
    bool typeDone = false;
    if(arguments.size() == 1){
    return;
    }
    if(builtins.contains(arguments[1])){
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

void executable(int redirectType, std::vector<std::string> arguments, std::vector<std::string> paths, std::string file){
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
      if(!file.empty()){
        if(redirectType == 1)
          int x = R_stdout(file);
        else if(redirectType == 2)
          int x = R_stderr(file);
        else if(redirectType == 3){
          int x = R_append(file);
        }
      }
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
