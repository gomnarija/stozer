#ifndef IZVRSI_H
#define IZVRSI_H

#include <stozer.h>
#include <termija.h>
#include <momo.h>
#include <argumentator.h>

#include <map>
#include <sstream>
#include <vector>
#include <unordered_map>

namespace stozer{

class Izvrsi : public Process{

private:
    bool                        shouldEnd;
    std::string                 izvrsiArguments;
    std::unordered_map<std::string, void (Izvrsi::*) (Argumentator&)>
                                argumentMap;


    termija::Pane       *pane=nullptr;
    //file name
    termija::Text       *fileNameText;
    uint8_t             maxFileNameLength = 16;
    //IO
    const std::string   fileExtension = ".txt";
    std::string         filePath;
    
    //momo
    momo::moEnv         globalEnv;

private:
    void            print_help();
    void            quit();

    //argument parsers
    void            execute_file(Argumentator&);


    //IO
    bool            isStozerFile(const std::string &);

    //executors
    void            executeSourceFile(const std::string &);
    
    //log
    void            printLog();
public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;
    void draw() override;

    Process*  instantiate(const std::string &) override;

public:
    Izvrsi(stozer::Stozer &, const std::string &);
};




}



#endif