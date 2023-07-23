
#include "listaj.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{

Listaj::Listaj(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                  = "listaj";//unique
    this->listajArguments       = arguments;
}


Process* Listaj::instantiate(const std::string &arguments){
    return new Listaj(this->stozer, arguments);
}




/*
    lists the content of the given directory
*/
void Listaj::setup(){
    this->isCommandProcess = true;
    std::vector<std::string> entries;
    //try to list the content of dir at given path,
    //  if there is a file at path instead of dir just print it's path
    if(!this->listajArguments.empty()){
        if(this->listajArguments.find(" ") == std::string::npos){//only path should be provided in arguments str
            std::string dofPath = this->stozer.getWorkingDirectory();
            if(filesystem::move_path(dofPath, this->listajArguments) &&//must be inside root
                filesystem::is_inside(this->stozer.getRootDirectory(), dofPath)){
                if(filesystem::is_dir(dofPath)){
                    //dir, list it
                    entries = this->stozer.listDirectory(dofPath);
                }else{
                    //file, print it's path
                    *(this->outStream)  << filesystem::relative_path(this->stozer.getRootDirectory(), dofPath);
                    return;
                }
            }else{
                //error: unknown path
                *(this->outStream)  << "putanja \"" + this->listajArguments + "\" nije pronadjena.";
                return; 
            }
        }else{
            //error: unknown argument
            std::string trailingArgument = string::split_string(this->listajArguments, " ").at(1);
            *(this->outStream)  << "argument \"" + trailingArgument + "\" nije prepoznat. pravilna upotreba komande je: listaj <putanja>.";
            return;
        }
    }else{
        entries = this->stozer.listDirectory(this->stozer.getWorkingDirectory());
    }
    //print entries
    *(this->outStream) << "tip  naziv" << "\n";
    *(this->outStream) << "***  *****" << "\n";
    for(const auto & entry : entries){
            *(this->outStream) << entry << "\n";
    }
}



void Listaj::cleanup(){
}

void Listaj::draw(){

}

void Listaj::update(){
    
}
}