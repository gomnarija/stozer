
#include "ukloni.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <argumentator.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{

Ukloni::Ukloni(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                  = "ukloni";//unique
    this->ukloniArguments       = arguments;
}

Process* Ukloni::instantiate(const std::string &arguments){
    return new Ukloni(this->stozer, arguments);
}




/*
    removes file or dir
    Syntax : <-arg> <path>
    Arguments:
        -p
        -s - delete non-empty dirs
*/
void Ukloni::setup(){
    this->isCommandProcess = true;
    bool isForced = false;
    //help
    if(this->ukloniArguments == "-p"){
        this->displayHelp();
        return;
    }
    //parse argumentString
    if(!this->ukloniArguments.empty()){
        Argumentator argumentator(this->ukloniArguments);
        //parse arguments
        for(const auto& arg : argumentator.getArguments()){
            if(arg == "s"){
                isForced = true;
            }else if(arg == "p"){//-p is handled above, so this could happen only if it's passed with some other args
                *(this->outStream)  << "nepravilno unesena komanda. unesi argument -p za pomoć.";
                return;
            }else{
                *(this->outStream)  << "argument \"" + arg + "\" nije prepoznat. unesi argument -p za pomoć.";
                return;
            }
        }
        //get old/new name values
        if(argumentator.getValues().size() != 1){
            //no old/new name pair provided
            *(this->outStream)  << "nepravilno unesena komanda. unesi argument -p za pomoć.";
            return;
        }
        std::string path = argumentator.getValues().at(0);
        
        int8_t res = this->stozer.removeFileOrDir(path, isForced);
        if(res == -1){
            *(this->outStream)  << "nemaš ovlašćenje za datu putanju.";
        }else if(res == 0){
            *(this->outStream)  << "greška pri uklanjanju. putanja je nevažeća.";
        }else if(res == -2){
            *(this->outStream)  << "direktorijum na putanji nije prazan. iskoristi -s da prisiliš uklanjanje."; 
        }
    }else{
        *(this->outStream)  << "putanja nije prosleđena. unesi argument -p za pomoc."; 
    }
}


void 
Ukloni::displayHelp(){
    *(this->outStream)  << "pomoc, todo";
}

void Ukloni::cleanup(){
}

void Ukloni::draw(){

}

void Ukloni::update(){
    
}
}