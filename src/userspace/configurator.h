#ifndef CONFIGURATOR_H
#define CONFIGURATOR_H

#include <string>
#include <vector>
#include <unordered_map>

namespace stozer{

/*
    used by processes for config files
*/
class Configurator{
private:
    std::string                     rawConfigString;
    std::unordered_map<std::string, std::string>
                                    keyValuePairMap;
    std::vector<std::string>        keys;

public:
    Configurator(const std::string &);
    Configurator(){}
    bool                                    loadFile(const std::string &);
    const std::vector<std::string>&         getKeys() const;
    std::string                             getValue(const std::string &) const;
};

}




#endif