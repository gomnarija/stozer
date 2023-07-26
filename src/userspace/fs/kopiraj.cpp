
#include "kopiraj.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <argumentator.h>

#include <plog/Log.h>
#include <regex>
#include <filesystem>

namespace stozer{


Kopiraj::Kopiraj(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                      = "kopiraj";//unique
    this->kopirajArguments          = arguments;
}

Process* Kopiraj::instantiate(const std::string &arguments){
    return new Kopiraj(this->stozer, arguments);
}




/*
    copies file or dir
    Syntax: kopiraj <old> <new>
*/
void Kopiraj::setup(){
    this->isCommandProcess = true;
    std::filesystem::copy_options copyOptions = std::filesystem::copy_options::recursive;
    //parse argumentString
    if(!this->kopirajArguments.empty()){
        Argumentator argumentator(this->kopirajArguments);
        //get old/new name values
        if(argumentator.getValues().size() != 2 || !argumentator.getArguments().empty()){
            //no old/new name pair provided
            *(this->outStream)  << "nepravilno unesena komanda. pravilna upotreba komande je: kopiraj <stara_putanja> <nova_putanja>.";
            return;
        }
        std::string oldName = argumentator.getValues().at(0);
        std::string newName = argumentator.getValues().at(1);
        
        int8_t res = this->stozer.copyFileOrDir(oldName, newName, copyOptions);
        if(res == -1){
            *(this->outStream)  << "nemas ovlascenje za datu putanju.";
        }else if(res == 0){
            *(this->outStream)  << "greska pri kopiranju. nevazeca putanja, ili na njoj nesto vec postoji.";
        }else if(res == -2){
            *(this->outStream)  << "greska pri kopiranju. novi naziv je nevazeci: " + filesystem::get_name(newName);
        }
    }else{
        *(this->outStream)  << "nazivi nisu prosledjeni. pravilna upotreba komande je: kopiraj <stara_putanja> <nova_putanja>."; 
    }
}


void Kopiraj::cleanup(){
}

void Kopiraj::draw(){

}

void Kopiraj::update(){
    
}
}