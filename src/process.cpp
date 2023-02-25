#include "stozer.h"
#include <termija.h>
#include <plog/Log.h>

namespace stozer{


Process::Process(Stozer &stozer):
stozer{stozer}
{}


const std::string & 
Process::getName(){
    return this->name;
}

uint16_t 
Process::getPID(){
    return this->PID;
}

void 
Process::setPID(uint16_t PID){
    if(this->PID==0)
        this->PID = PID;        
}


}