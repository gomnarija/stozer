#ifndef KRSH_H
#define KRSH_H

#include <stozer.h>
#include <termija.h>


namespace stozer{

class Krsh : public Process{
private:
    termija::Pane               *pane;
    termija::TextBox            *textBox;
public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate() override;

public:
    Krsh(stozer::Stozer &);

};




}



#endif