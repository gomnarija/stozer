
#include "listaj.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <argumentator.h>

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
        //help
        if(this->listajArguments == "-p"){
            this->displayHelp();
            return;
        }

        Argumentator argumentator(this->listajArguments);
        //parse arguments
        for(const auto &arg : argumentator.getArguments()){
            if(arg == "s"){
                this->showHidden = true;
            }else if(arg == "p"){//-p is handled above, so this could happen only if it's passed with some other args
                *(this->outStream)  << "nepravilno unesena komanda. unesi argument -p za pomoć.";
                return;
            }else{
                *(this->outStream)  << "argument \"" + arg + "\" nije prepoznat. unesi argument -p za pomoć.";
                return;
            }
        }
        //listaj
        if(argumentator.getValues().size() == 1){//only one path value should be provided
            std::string dofPath = this->stozer.getWorkingDirectory();
            std::string path = argumentator.getValues().at(0);
            if(filesystem::move_path(dofPath, path, this->stozer.getRootDirectory()) &&//must be inside root
                filesystem::is_inside(this->stozer.getRootDirectory(), dofPath)){
                if(filesystem::is_dir(dofPath)){
                    //dir, list it
                    entries = this->stozer.listDirectory(dofPath);
                    if(!this->showHidden){
                        this->filterHidden(entries);
                    }
                }else{
                    //file, print it's path
                    *(this->outStream)  << filesystem::relative_path(this->stozer.getRootDirectory(), dofPath);
                    return;
                }
            }else{
                //error: unknown path
                *(this->outStream)  << "putanja \"" + path + "\" nije pronadjena.";
                return; 
            }
        }else if(argumentator.getValues().size() == 0){
            entries = this->stozer.listDirectory(this->stozer.getWorkingDirectory());
            if(!this->showHidden){
                this->filterHidden(entries);
            }
        }
        else{
            //error: unknown argument
            *(this->outStream)  << "pogrešna upotreba komande. unesi argument -p za pomoć.";
            return;
        }
    }else{
        entries = this->stozer.listDirectory(this->stozer.getWorkingDirectory());
        if(!this->showHidden){
            this->filterHidden(entries);
        }
    }
    //print entries
    *(this->outStream) << "tip  naziv" << "\n";
    *(this->outStream) << "***  *****" << "\n";
    for(const auto & entry : entries){
            *(this->outStream) << entry << "\n";
    }
}


void 
Listaj::displayHelp(){
    *(this->outStream)  << "pomoc, todo";
}

void 
Listaj::filterHidden(std::vector<std::string> &entries){
    for(int i=1;i<entries.size();i++){
        std::string name = string::split_string_into_two(string::remove_extra_whitespace(entries.at(i)), " ").second;
        if(name.size() > 0 && name.at(0) == this->hiddingChar){
            entries.erase((entries.begin() + i));
            i--;
        }
    }
}


void Listaj::cleanup(){
}

void Listaj::draw(){

}

void Listaj::update(){
    
}
}