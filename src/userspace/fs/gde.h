#ifndef GDE_H
#define GDE_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Gde : public Process{

private:
    std::string         gdeArguments;

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Gde(stozer::Stozer &, const std::string &);
};




}



#endif