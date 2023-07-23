#ifndef UKLONI_H
#define UKLONI_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Ukloni : public Process{

private:
    std::string         ukloniArguments;


    void displayHelp();

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Ukloni(stozer::Stozer &, const std::string &);
};




}



#endif