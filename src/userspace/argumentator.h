#ifndef ARGUMENTATOR_H
#define ARGUMENTATOR_H


#include <string>
#include <vector>

namespace stozer{

/*
    used by processes for handling passed arguments
*/
class Argumentator{
private:
    std::string                     rawArgumentString;
    std::vector<std::string>        tokens;
    std::vector<std::string>        arguments;
    std::vector<std::string>        values;

public:
    Argumentator(std::string);

    const std::vector<std::string>&        getArguments() const;
    const std::vector<std::string>&        getValues() const;

};

}


#endif