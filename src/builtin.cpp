#include "builtin.h"
#include "redirect.h"
#include <iostream>
#include <filesystem>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

void echo(const std::vector<std::string>& arguments){
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

void pwd(const std::string& current_directory){
    std::cout << current_directory << std::endl;
}

void cd(const std::vector<std::string>& arguments, std::string& current_directory){
  if(arguments.size() == 1 || arguments[1] == "~"){
    const char* env_HOME {std::getenv("HOME")};
    std::string HOME{env_HOME ? env_HOME : ""};
    if(HOME.empty()){
      std::cerr << "cd: HOME not set\n";
      return;
    }
    try{
      std::filesystem::current_path(HOME);
      current_directory = HOME;
    }
    catch(const std::filesystem::filesystem_error& e){
      std::cerr << "Error: " << e.what() << "\n";
      return;
    }   
  }

  try{
    if( std::filesystem::is_directory(arguments[1])){
      std::string errorHandlingPath = std::filesystem::canonical(arguments[1]).string();
      std::filesystem::current_path(errorHandlingPath);
      current_directory = errorHandlingPath;
      return;
    }
  }
  catch(const std::filesystem::filesystem_error& e){
    std::cerr << "Error: " << e.what() << "\n";
    return;
  }
  std::cerr << "cd: " << arguments[1] << ": No such file or directory" << std::endl;
}

void type(const std::vector<std::string>& arguments, const std::vector<std::string>& paths, 
    const std::unordered_set<std::string>& builtins){
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
      std::cerr << arguments[1] << ": not found" << std::endl;
    }
}

void executable(int redirectType, std::vector<std::string>& arguments, 
  const std::vector<std::string>& paths, const std::string& file){
  std::string exCommand = arguments[0];
  std::string fullpath = find_external_file_path(exCommand, paths);

  if(!fullpath.empty()){
    std::vector<char*> arguments_c;
    for(std::string& s: arguments){
      arguments_c.push_back(s.data());
    }
    arguments_c.push_back(nullptr);
    pid_t pid = fork();
    if(pid == -1){
        perror("fork failed");
        return;
    }
    if(pid == 0){
      int redirectOk = 0;
      if(!file.empty()){
        if(redirectType == 1)
          redirectOk = R_stdout(file);
        else if(redirectType == 2)
          redirectOk = R_stderr(file);
        else if(redirectType == 3){
          redirectOk = R_append(file);
        }
        else if(redirectType == 4){
          redirectOk = R_append_stderr(file);
        }
      }
      if(redirectOk == -1)
        std::exit(1);
      int execvErr {0};
      execvErr = execv(fullpath.c_str(), arguments_c.data());
      if(execvErr == -1){
        perror(fullpath.c_str());
      }
      std::exit(1);
    }
    else{
      waitpid(pid, nullptr, 0);
    }
  }
  else{
    std::cerr << arguments[0] << ": command not found" << std::endl;
  }
}

std::string find_external_file_path(const std::string& cmd, const std::vector<std::string>& paths){
  std::string fullpath;
  for (const std::string& p : paths){ 
    fullpath = p + "/" + cmd;
    if (std::filesystem::is_regular_file(fullpath) && access (fullpath.c_str(), X_OK) == 0){
      return fullpath;
    } 
  } 
  return "";
}
