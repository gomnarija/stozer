#ifndef POMERI_H
#define POMERI_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Pomeri : public Process{

private:
    std::string         pomeriArguments;

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;



public:
    Pomeri(stozer::Stozer &, const std::string &);
};




}



#endif