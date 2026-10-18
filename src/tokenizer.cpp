#include "tokenizer.h"

std::vector<std::string> tokenize(const std::string& command, const char spliter){
  int QuoteStatus = 0;
  std::vector<std::string> arguments;
  std::string arg;

  for(size_t i = 0; i < command.length(); i++){
    if(command[i] == '\\'){
      if(QuoteStatus == 0 && i + 1 < command.length()){
        i++;
        arg.push_back(command[i]);
      }
      else{
        if((command[i + 1] == '\\'|| command[i + 1] == '"') && QuoteStatus == 2){
          i++;
          arg.push_back(command[i]);
        }
        else{
          arg.push_back(command[i]);
        }
      }
    }
    else if(command[i] == '\''){
      if(QuoteStatus == 1){
        QuoteStatus = 0;
      }
      else if(QuoteStatus == 2){
        arg.push_back(command[i]);
      }
      else{
        QuoteStatus = 1;
      }
    }
    else if(command[i] == '"'){
      if(QuoteStatus == 2){
        QuoteStatus = 0;
      }
      else if(QuoteStatus == 1){
        arg.push_back(command[i]);
      }
      else{
        QuoteStatus = 2;
      }
    }
    else if(QuoteStatus == 1 || QuoteStatus == 2){
      arg.push_back(command[i]);
    }
    else if(command[i] == spliter){
      if(!arg.empty()){
        arguments.push_back(arg);
        arg.clear();
      }
    }
    else{
      arg.push_back(command[i]);
    }
  }
  if(!arg.empty()) arguments.push_back(arg);

  return arguments;
}
