#include "izvrsi.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <argumentator.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{

Izvrsi::Izvrsi(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                  = "izvrši";//unique
    this->izvrsiArguments       = arguments;
}

Process* Izvrsi::instantiate(const std::string &arguments){
    return new Izvrsi(this->stozer, arguments);
}




/*
    moves working directory to given path
*/
void Izvrsi::setup(){ 
    this->isCommandProcess = true;



    //help
    if(this->izvrsiArguments == "-p"){
        this->print_help();
        return;
    }

    //arguments
    this->argumentMap.emplace("f", &Izvrsi::execute_file);


    //commands, TODO:add config

    //parse argumentString
    if(!this->izvrsiArguments.empty()){
        Argumentator argumentator(this->izvrsiArguments);
        //parse arguments
        for(const auto &arg : argumentator.getArguments()){
            if(this->argumentMap.find(arg) == this->argumentMap.end()){
                    //unknown arg
                    *(this->outStream)  << "argument '" << arg << "' nije prepoznat. unesi argument -p za pomoć.";
                    this->shouldEnd = true;
                    return;
                }else{
                    //call arg function
                    (this->*(this->argumentMap.at(arg)))(argumentator);//sacuvaj boze
            }
        }
    }else{
        //TODO
    }


    //TUI
    // termija::Termija &termija = termija::tra_get_instance();
    // //create pane
    // this->pane = this->stozer.createPane(this->PID);

    // //IO
    //     //load from file

    // //all good; set to front
    // this->stozer.setFrontPID(this->PID);
}

void Izvrsi::cleanup(){
    //TODO: cleanup widgets
    if(this->pane != nullptr)
        termija::tra_remove_pane(this->pane);

}

void Izvrsi::draw(){
}

void Izvrsi::update(){
    if(shouldEnd){
        this->stozer.processTerminate(this->PID);
        return;
    }
}


void
Izvrsi::print_help(){
    *(this->outStream)  << "pomoc todo";
    this->shouldEnd = true;
}


/*
    executes file at given path,
        prints out last returned value
*/
void
Izvrsi::execute_file(Argumentator &argumentator){
    //only one argument
    if(argumentator.getArguments().size() != 1){
        *(this->outStream)  << "pogrešno uneseni argumenti. unesi argument -p za pomoć.";
        this->shouldEnd = true;
        return;
    }
    //only one path value
    if(argumentator.getValues().size() != 1){
        *(this->outStream)  << "previše putanja je uneseno. unesi argument -p za pomoć.";
        this->shouldEnd = true;
        return;
    }
    //get path
    std::string relativePath = argumentator.getValues().at(0);
    std::string path = this->stozer.getWorkingDirectory();
    //check if it exists
    if(filesystem::move_path_wv(path, relativePath, this->stozer.getRootDirectory())
                && std::filesystem::is_regular_file(filesystem::path_to_u32(path + this->fileExtension))){

        //single source file
        if(this->isStozerFile(path)){
            this->executeSourceFile(path);
            this->shouldEnd = true;
            return;    
        }
        //TODO build files

    }else{
        *(this->outStream)  << "nevažeća putanja. unesi argument -p za pomoć.";
        this->shouldEnd = true;
        return;
    }

}





/*
    prints momo LOG_STREAM to outstream,
        cleaning it after
*/
void
Izvrsi::printLog(){
    if(momo::LOG_STREAM.rdbuf()->in_avail() > 0){
        *(this->outStream)  << momo::LOG_STREAM.str();
        momo::LOG_STREAM.str(std::string());;
    }
}


//
//  COMMANDS
//

void
Izvrsi::quit(){
    this->stozer.processTerminate(this->PID);
}



}