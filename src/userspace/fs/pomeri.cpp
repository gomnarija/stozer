
#include "pomeri.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <argumentator.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{


Pomeri::Pomeri(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                  = "pomeri";//unique
    this->pomeriArguments       = arguments;
}

Process* Pomeri::instantiate(const std::string &arguments){
    return new Pomeri(this->stozer, arguments);
}




/*
    renames or moves file or dir
    Syntax: pomeri <old> <new>
*/
void Pomeri::setup(){
    this->isCommandProcess = true;
    //parse argumentString
    if(!this->pomeriArguments.empty()){
        Argumentator argumentator(this->pomeriArguments);
        //get old/new name values
        if(argumentator.getValues().size() != 2 || !argumentator.getArguments().empty()){
            //no old/new name pair provided
            *(this->outStream)  << "nepravilno unesena komanda. pravilna upotreba komande je: pomeri <stara_putanja> <nova_putanja>.";
            return;
        }
        std::string oldName = argumentator.getValues().at(0);
        std::string newName = argumentator.getValues().at(1);
        
        int8_t res = this->stozer.moveFileOrDir(oldName, newName);
        if(res == -1){
            *(this->outStream)  << "nemaš ovlašćenje za datu putanju.";
        }else if(res == 0){
            *(this->outStream)  << "greška pri pomeranju. nevažeća putanja, ili na njoj nesto već postoji.";
        }else if(res == -2){
            *(this->outStream)  << "greška pri pomeranju. novi naziv je nevažeći: " + filesystem::get_name(newName);
        }
    }else{
        *(this->outStream)  << "nazivi nisu prosleđeni. pravilna upotreba komande je: pomeri <stara_putanja> <nova_putanja>."; 
    }
}





void Pomeri::cleanup(){
}

void Pomeri::draw(){

}

void Pomeri::update(){
    
}
}