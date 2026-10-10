#pragma once
#include "builtin.h"

bool check_pipeline(std::vector<std::string> arguments){
    for(std::string s : arguments){
        if(s == "|")
            return true;
    }
    return false;
}

void pipeline(std::vector<std::string> arguments, std::vector<std::string> paths,
    std::unordered_set<std::string> builtins, std::string current_directory){
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> cmd0;
    bool cmd_decider {true};
    int j {0};
    for(int i = 0; i < arguments.size(); i++){
        cmd0.push_back(arguments[i]);
        if(i + 1 == arguments.size() || arguments[i + 1] == "|"){
            commands.push_back(cmd0);
            cmd0.clear();
            j++;
            i++;
        }  
    }
    
    std::vector<char*> arguments_c;
    std::vector<int> PIDs;
    int fds[2];
    int prevRead;

    for(int i = 0; i < commands.size(); i++){
        if(i != 0){
            prevRead = fds[0];
        }
        if(i != commands.size() - 1){
            pipe(fds);
        }
        pid_t pid = fork();
        if(pid == 0){
            if(i == 0){
                dup2(fds[1], STDOUT_FILENO); 
                close(fds[1]);
                close(fds[0]);
            }
            else if(i == commands.size() - 1){
                dup2(prevRead, STDIN_FILENO);
                close(prevRead);
            }
            else{
                dup2(prevRead, STDIN_FILENO);
                dup2(fds[1], STDOUT_FILENO);
                close(prevRead);
                close(fds[1]);
                close(fds[0]);
            }
            if(!builtins.contains(commands[i][0])){
                std::string fullpath = find_external_file_path(commands[i][0], paths);
                if(!fullpath.empty()){
                    for(std::string& s: commands[i]){
                        arguments_c.push_back(s.data());
                    }
                    arguments_c.push_back(nullptr);
                    execv(fullpath.c_str(), arguments_c.data());
                }
                else{
                    std::cerr << commands[i][0] << ": command not found" << std::endl;
                }
            }
            else{
                if (commands[i][0] == "echo")
                    echo(commands[i]);
                else if (commands[i][0] == "pwd")
                    pwd(current_directory);
                else if (commands[i][0] == "cd")
                    cd(commands[i], current_directory);
                else if (commands[i][0] == "type")
                    type(commands[i], paths, builtins);
            }
            std::exit(1);
        }
        if(i != commands.size() - 1){
            close(fds[1]);
        }
        PIDs.push_back(pid); 
        if(i != 0){
            close(prevRead);
        }
    }
    for(int p : PIDs){
        waitpid(p, nullptr, 0);
    }
    
}


