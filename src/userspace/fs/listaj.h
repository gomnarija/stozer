#ifndef LISTAJ_H
#define LISTAJ_H

#include <stozer.h>
#include <termija.h>

#include <sstream>

namespace stozer{

class Listaj : public Process{

private:
    std::string         listajArguments;
    bool                showHidden=false;
    char                hiddingChar = '_';

    void                displayHelp();
    void                filterHidden(std::vector<std::string> &);

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Listaj(stozer::Stozer &, const std::string &);
};




}



#endif