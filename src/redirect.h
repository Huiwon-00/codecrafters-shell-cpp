#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

int R_stdout(std::string file){        
    int fd = open(file.c_str(), O_CREAT | O_TRUNC, 0644);
    int restore_fd = dup(1);
    dup2(fd, 1);
    close(fd);
    return restore_fd;
}

void restore(int restore_fd){
    dup2(restore_fd, 1);
    close(restore_fd);
}

std::string remove_redirect_sign(std::vector<std::string> arguments){
    std::string file;
    for(int i = 0; i < arguments.size(); i++){
        if (arguments[i] == ">"){
            if(i < arguments.size() - 1){
                file = arguments[i + 1];
                arguments.erase(arguments.begin() + i, arguments.begin() + i + 2);
                break;
            }
            else{
                arguments.erase(arguments.begin() + i);
                break;
            }
        }
    }
    return file;
}