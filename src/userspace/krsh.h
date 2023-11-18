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
    uint16_t                    maxHistoryLength        = 128;

//user config
    std::string                 configFileRelativePath  = "/korisnici/rs26/_krsh/postavke";
    std::string                 historyFileRleativePath = "/korisnici/rs26/_krsh/istorija";
    const char*                 fileExtension = ".txt";



private:
    termija::Pane                                   *pane;
    termija::TextBox                                *textBox;
    size_t                                          commandStartIndex;
    std::stringstream                               krshOutStream;
    std::unordered_map<std::string, std::string>    configMap;//values to be used, mainly in handle
    std::string                                     handle = "%ime@stožer>";//default
    std::map<uint16_t, bool>                        PIDMap;
    std::vector<std::string>                        historyVector;
    size_t                                          historyIndex;
    std::string                                     cachedCommand;//unfinished command before starting scrolling history


    int8_t                          built_in_commands(const std::string &, const std::string &);
    void                            load_config_file();
    void                            load_history_file();
    void                            save_history_to_file();

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