#pragma once
#include <iostream>
#include <string>
#include <unordered_set>
#include <filesystem>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <utility>

int R_stdout(std::string file){        
    int fd = open(file.c_str(), O_CREAT | O_TRUNC| O_WRONLY, 0644);
    int restore_fd = dup(1);
    dup2(fd, 1);
    close(fd);
    return restore_fd;
}
int R_stderr(std::string file){
    int fd = open(file.c_str(), O_CREAT | O_TRUNC| O_WRONLY, 0644);
    int restore_fd = dup(2);
    dup2(fd, 2);
    close(fd);
    return restore_fd;
}

int R_append(std::string file){
    int fd = open(file.c_str(), O_CREAT | O_APPEND| O_WRONLY, 0644);
    int restore_fd = dup(1);
    dup2(fd, 1);
    close(fd);
    return restore_fd;
}

int R_append_stderr(std::string file){
    int fd = open(file.c_str(), O_CREAT | O_APPEND| O_WRONLY, 0644);
    int restore_fd = dup(2);
    dup2(fd, 2);
    close(fd);
    return restore_fd;
}

void restore(int restore_fd, int redirectType){
    int restor_to;
    if(redirectType == 1 || redirectType == 3){
        restor_to = 1;
    }
    else{
        restor_to = 2;
    }
    dup2(restore_fd, restor_to);
    close(restore_fd);
}

std::pair<int, std::string> remove_redirect_sign(std::vector<std::string>& arguments){
    std::string file;
    int redirectType = 0;
    for(int i = 0; i < arguments.size(); i++){
        if (arguments[i] == ">" || arguments[i] == "1>" || arguments[i] == "2>" || 
            arguments[i] == "1>>" || arguments[i] == ">>" || arguments[i] == "2>>"){
            if(arguments[i] == "2>"){
                redirectType = 2;
            }
            else if(arguments[i] == ">"|| arguments[i] == "1>"){
                redirectType = 1;
            }
            else if(arguments[i] == "2>>"){
                redirectType = 4;
            }
            else{
                redirectType = 3;
            }
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
    return {redirectType, file};
}