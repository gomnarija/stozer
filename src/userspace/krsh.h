#ifndef KRSH_H
#define KRSH_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>

namespace stozer{

class Krsh : public Process{

//config
private:
    uint8_t                     textBoxMargin           = 3;//percentage points
    uint16_t                    maxCommandLength        = 256;//maximum allowed command length
    uint16_t                    maxTextBoxTextLength    = 32768;//maximum allowed textBox text length, clears if over


//user config
    std::string                 configFileRelativePath  = "/korisnici/rs26/_krsh";





private:
    termija::Pane                                   *pane;
    termija::TextBox                                *textBox;
    size_t                                          commandStartIndex;
    std::stringstream                               krshOutStream;
    std::unordered_map<std::string, std::string>    configMap;//values to be used, mainly in handle
    std::string                                     handle = "%ime@stožer>";//default
    std::map<uint16_t, bool>                        PIDMap;


    int8_t                          built_in_commands(const std::string &, const std::string &);

public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Krsh(stozer::Stozer &, const std::string &);
};




}



#endif