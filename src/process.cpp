#include "stozer.h"
#include <termija.h>
#include <plog/Log.h>

namespace stozer{



Process::Process(Stozer &stozer, const std::string &arguments):
stozer{stozer}
{}


const std::string & 
Process::getName() const{
    return this->name;
}

uint16_t 
Process::getPID() const{
    return this->PID;
}

void 
Process::setPID(uint16_t PID){
    if(this->PID==0)
        this->PID = PID;        
}

void
Process::setOutStream(std::stringstream *os){
    this->outStream = os;
}

bool
Process::isBackground() const{
    return this->isBackgroundProcess;
}

bool
Process::isCommand() const{
    return this->isCommandProcess;
}

}