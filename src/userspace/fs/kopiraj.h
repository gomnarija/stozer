#ifndef KOPIRAJ_H
#define KOPIRAJ_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Kopiraj : public Process{

private:
    std::string         kopirajArguments;

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;



public:
    Kopiraj(stozer::Stozer &, const std::string &);
};




}



#endif