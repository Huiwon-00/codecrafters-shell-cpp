#pragma once
#include "redirect.h"

std::string find_external_file_path(std::string cmd, std::vector<std::string> paths);

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
        std::string fullpath = find_external_file_path(arguments[1], paths);
        if(!fullpath.empty()){
            std::cout << arguments[1] << " is " << fullpath << std::endl;
            typeDone = true;
        }
    }
        if(!typeDone){
        std::cout << arguments[1] << ": not found" << std::endl;
    }
}

void executable(int redirectType, std::vector<std::string> arguments, std::vector<std::string> paths, std::string file){
  std::string exCommand = arguments[0];
  std::string fullpath = find_external_file_path(exCommand, paths);

  if(!fullpath.empty()){
    std::vector<char*> arguments_c;
    for(std::string& s: arguments){
      arguments_c.push_back(s.data());
    }
    arguments_c.push_back(nullptr);
    pid_t pid = fork();
    if(pid == 0){
      if(!file.empty()){
        if(redirectType == 1)
          R_stdout(file);
        else if(redirectType == 2)
          R_stderr(file);
        else if(redirectType == 3){
          R_append(file);
        }
        else if(redirectType == 4){
          R_append_stderr(file);
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

std::string find_external_file_path(std::string cmd, std::vector<std::string> paths){
  std::string fullpath;
  for (std::string p : paths){ 
    fullpath = p + "/" + cmd;
    //std::cout << fullpath << std::endl;
    if (std::filesystem::is_regular_file(fullpath) && access (fullpath.c_str(), X_OK) == 0){
      return fullpath;
    } 
  } 
  return "";
}
