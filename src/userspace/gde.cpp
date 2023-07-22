
#include "gde.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{

Gde::Gde(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name              = "gde";//unique
    this->gdeArguments      = arguments;
}

Process* Gde::instantiate(const std::string &arguments){
    return new Gde(this->stozer, arguments);
}




/*
    no arguments, prints current working directory
*/
void Gde::setup(){
    this->isCommandProcess = true;
    if(this->gdeArguments.empty()){
        *(this->outStream)  << this->stozer.getWorkingDirectoryRelative();
    }else{
        std::string firstArg = string::split_string_into_two(this->gdeArguments, " ").first;
        *(this->outStream)  << "Argument \"" + firstArg + "\" nije prepoznat.";
    }
}



void Gde::cleanup(){
}

void Gde::draw(){

}

void Gde::update(){
    
}
}