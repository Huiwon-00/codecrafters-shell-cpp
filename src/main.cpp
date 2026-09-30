#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <unistd.h>

namespace fs = std::filesystem;

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

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
  /*
  for(std::string p : paths) {
		std::cout << p << std::endl;
	}
  */

  // TODO: Uncomment the code below to pass the first stage 
  while(true){
    std::cout << "$ ";
    std::string command;
    std::unordered_set<std::string> types = {"exit", "echo", "type"};
    std::getline(std::cin, command);
    if( command == "exit"){
      std::cout << command << std::endl;
      break;
    }
    else if(command.substr(0, 4) == "echo"){
      std::cout << command.substr(5) << std::endl;
    }
    else if(command.substr(0, 4) == "type"){
      bool typeDone = false;
      if(types.contains(command.substr(5))){
        std::cout << command.substr(5) << " is a shell builtin" << std::endl;
        typeDone = true;
      }
      else if( !typeDone ){
        std::string fullpath ;
        for (std::string p : paths){ 
          fullpath = p + "\\" + command.substr(5);
          //std::cout << fullpath << std::endl;
          if (access (fullpath.c_str(), X_OK) == 0){
            std::cout << command.substr(5) << "is" << fullpath << std::endl;
            typeDone = true;
            break;
          } 
        } 
        //std::cout << typeDone << std::endl;
      }
      if(!typeDone){
        std::cout << command.substr(5) << ": not found" << std::endl;
      }
    }
    else std::cout << command << ": command not found" << std::endl;
  }

  
}




/*
      for(const auto& entry: fs::directory_iterator(filepath)){
          if(entry == command.substr(5)){
            std::cout << "Path: " << entry.path() << "\n";
          }
        }
      
      else if( command.substr(5) ){
        ///Check if the file is in given PATH
      }
      */