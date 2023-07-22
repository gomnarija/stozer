#ifndef IDI_H
#define IDI_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Idi : public Process{

private:
    std::string         idiArguments;

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Idi(stozer::Stozer &, const std::string &);
};




}



#endif