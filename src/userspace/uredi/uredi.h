#ifndef UREDI_H
#define UREDI_H

#include <stozer.h>
#include <termija.h>

#include <map>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace stozer{

class Uredi : public Process{

private:
    bool                shouldEnd=false;

    std::string         urediArguments;
    std::unordered_map<std::string, void (Uredi::*) ()>
                        argumentMap;
    termija::Pane       *pane;
    //box
    termija::Box        *box;
    uint8_t             boxMargin = 1;//percentage of pane size
    //options section
    std::vector<std::pair<std::string, 
                std::string>>       optionsSectionEntries;
    uint8_t                         optionsSectionHeightMargin = 1;//in text size, applied before and after
    uint8_t                         optionsSectionWidthMargin = 2;//in text size, applied before and after
    uint8_t                         entriesPerRow = 3;
    uint8_t                         optionsSectionRows=1;//counter
    std::vector<termija::Text*>     optionsSectionTextVector;  
    //file name
    termija::Text       *fileNameText;
    uint8_t             maxFileNameLength = 16;
    //textBox
    termija::TextBox    *textBox;
    uint8_t             textBoxMargin = 1;//percentage of box size;
    //IO
    const std::string   fileExtension = ".txt";
    const std::string   defaultFileName = "nepoznato";
    std::string         filePath;
    //input
    std::unordered_map<KeyboardKey, void (Uredi::*) ()>
                        controlKeysMap;
    

private:
    void            print_help();
    //TUI
    void            setup_box();
    void            setup_options_section();
    void            setup_file_name();
    void            setup_text_box();
    uint16_t        get_options_section_height();
    //IO
    const std::string       &getFileName();
    bool                    loadFromFile(const std::string &);
    bool                    saveToFile(const std::string &);
    bool                    fileExists(const std::string &);
    //Input
    void            handle_key(KeyboardKey);
    void            handle_text(std::string &);
    void            handle_command(KeyboardKey);
    //commands
    void            save_file();
public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Uredi(stozer::Stozer &, const std::string &);
};




}



#endif