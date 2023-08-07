#include "configurator.h"
#include <utils.h>

#include <fstream>
#include <filesystem>

namespace stozer{

Configurator::Configurator(const std::string &filePath){
    this->loadFile(filePath);
}



const std::vector<std::string>&
Configurator::getKeys() const{
    return this->getKeys();
}

/*
    returns value for given key,
        if there is none returns empty string
*/
std::string
Configurator::getValue(const std::string &key) const{
    if(this->keyValuePairMap.find(key) != this->keyValuePairMap.end()){
        return this->keyValuePairMap.at(key);
    }else{
        return "";
    }
}

/*
    loads and parses given file
*/
bool
Configurator::loadFile(const std::string &filePath){
    std::ifstream file(std::filesystem::u8path(filePath));
    if(!file.is_open())
        return false;

    //read line by line
    //  key = value
    std::string line;
    while(std::getline(file, line)){
        std::pair<std::string, std::string> keyValuePair = string::split_string_into_two(line, "=");
        if(keyValuePair.first == "" || keyValuePair.second == "")
            continue;
        //remove whitespace from key
        std::string key = string::replace_all(keyValuePair.first, " ", "");
        if(key.at(0)=='#')//comment
            continue;
        //add into key vector
        this->keys.push_back(key);
        //add into key/value map
        this->keyValuePairMap.emplace(key, keyValuePair.second);
    }

    //done
    file.close();
    return !file.is_open();
}


}