#include "argumentator.h"
#include <utils.h>

namespace stozer{

Argumentator::Argumentator(std::string argumentString){
    this->rawArgumentString = argumentString;
    this->tokens = string::split_string(argumentString, " ");
    for(const auto &token : tokens){
        if(token.empty())
            continue;

        if(token.at(0) == '-'){
            this->arguments.push_back(token.substr(1));
        }else{
            this->values.push_back(token);
        }
    }
}


const std::vector<std::string>&
Argumentator::getArguments() const{
    return this->arguments;
}

const std::vector<std::string>&
Argumentator::getValues() const{
    return this->values;
}


}