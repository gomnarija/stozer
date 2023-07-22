
#include "idi.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{

Idi::Idi(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name              = "idi";//unique
    this->idiArguments      = arguments;
}

Process* Idi::instantiate(const std::string &arguments){
    return new Idi(this->stozer, arguments);
}




/*
    moves working directory to given path
*/
void Idi::setup(){
    this->isCommandProcess = true;

    if(!this->idiArguments.empty()){
        if(this->idiArguments.find(" ") == std::string::npos){//only path should be provided in arguments str
            if(!this->stozer.changeWorkingDirectory(this->idiArguments)){
                *(this->outStream)  << "putanja \"" + this->idiArguments + "\" nije pronadjena, ili se na njoj ne nalazi direktorijum."; 
            }
        }else{
            std::string trailingArgument = string::split_string(this->idiArguments, " ").at(1);
            *(this->outStream)  << "argument \"" + trailingArgument + "\" nije prepoznat. pravilna upotreba komande je: idi <putanja>.";
        }
    }else{
        //stay here, do nothing
    } 
}



void Idi::cleanup(){
}

void Idi::draw(){

}

void Idi::update(){
    
}
}