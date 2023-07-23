
#include "napravi.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{
Napravi::Napravi(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                  = "napravi";//unique
    this->napraviArguments      = arguments;
}

Process* Napravi::instantiate(const std::string &arguments){
    return new Napravi(this->stozer, arguments);
}


/*
    creates new file or directory at given path
    Syntax: napravi <arg0> <path0> <arg1> <path1> ...
    Note: arg is optional, creates file by default
    Arguments: 
        -d - create dir
        -f - create file
        -p - help
*/
void Napravi::setup(){
    this->isCommandProcess = true;
    std::vector<std::string> files, dirs;

    //help
    if(this->napraviArguments == "-p"){
        this->displayHelp();
        return;
    }

    //parse arguments
    if(!this->napraviArguments.empty()){
        std::vector<std::string> tokens = string::split_string(this->napraviArguments, " ");
        std::string lastToken;
        for(const auto &token : tokens){
            //arg
            if(token.at(0) == '-'){
                //there shouldn't be two args in a row
                if(lastToken.empty() || lastToken.at(0) != '-'){
                    lastToken = token;
                    continue;
                }else{
                    *(this->outStream)  << "nepravilno uneseni argumenti. unesi argument -p za pomoc.";
                    return;
                }
            }else{
            //name
                //check lastToken to see if this should be file or dir
                if(!lastToken.empty() && lastToken.at(0) == '-'){//arg
                    if(lastToken == "-f"){
                        //file
                        files.push_back(token);
                        lastToken = token;
                    }else if(lastToken == "-d"){
                        //dir
                        dirs.push_back(token);
                        lastToken = token;
                    }else if(lastToken == "-p"){
                        //wrong use of help argument
                        *(this->outStream)  << "nepravilno uneseni argumenti. unesi argument -p za pomoc.";
                        return;
                    }
                    else{
                        //unknown arg
                        *(this->outStream)  << "argument \"" + lastToken + "\" nije prepoznat. unesi argument -p za pomoc.";
                        return;
                    }
                }else{
                    //last token was name also, so it will create file which is default behaviour
                    files.push_back(token);
                    lastToken = token;
                }
            }
        }
        //arg can't be last
        if(!lastToken.empty() && lastToken.at(0) == '-'){
            *(this->outStream)  << "nepravilno uneseni argumenti. unesi argument -p za pomoc.";
            return;
        }
    }else{
        *(this->outStream)  << "naziv nije prosledjen. unesi argument -p za pomoc.";
        return;
    }
    //create files
    for(const auto &file : files){
        int8_t res = this->stozer.makeFile(file);
        std::string path = this->stozer.getWorkingDirectory();
        filesystem::move_path_wv(path, file,  this->stozer.getRootDirectory());
        path = filesystem::relative_path(this->stozer.getRootDirectory(), path);
        if(!filesystem::is_inside(this->stozer.getRootDirectory(), path))
            path = file;

        if(res == -1){
            *(this->outStream)  << "nemas ovlascenje za datu putanju: " + path << "\n";
        }else if(res == 0){
            *(this->outStream)  << "greska pri kreiranju novog fajla: " + path << "\n";
        }
    }
    //create dirs
    for(const auto &dir : dirs){
        int8_t res = this->stozer.makeDirectory(dir);
        std::string path = this->stozer.getWorkingDirectory();
        filesystem::move_path_wv(path, dir, this->stozer.getRootDirectory());
        path = filesystem::relative_path(this->stozer.getRootDirectory(), path);
        if(!filesystem::is_inside(this->stozer.getRootDirectory(), path))
            path = dir;

        if(res == -1){
            *(this->outStream)  << "nemas ovlascenje za datu putanju: " + path << "\n";
        }else if(res == 0){
            *(this->outStream)  << "greska pri kreiranju novog direktorijuma: " + path << "\n";
        }
    }
}


void 
Napravi::displayHelp(){
    *(this->outStream)  << "pomoc, todo";
}


void Napravi::cleanup(){
}

void Napravi::draw(){

}

void Napravi::update(){
    
}
}