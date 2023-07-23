#ifndef NAPRAVI_H
#define NAPRAVI_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Napravi : public Process{

private:
    std::string         napraviArguments;

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Napravi(stozer::Stozer &, const std::string &);
};




}



#endif