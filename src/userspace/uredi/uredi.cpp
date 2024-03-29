
#include "uredi.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <argumentator.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{

Uredi::Uredi(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name                  = "uredi";//unique
    this->urediArguments        = arguments;
}

Process* Uredi::instantiate(const std::string &arguments){
    return new Uredi(this->stozer, arguments);
}




/*
    moves working directory to given path
*/
void Uredi::setup(){ 
    this->isCommandProcess = false;



    //arguments
    this->argumentMap.emplace("p", &Uredi::print_help);


    //commands, TODO:add config
    this->controlKeysMap.emplace(KEY_S, &Uredi::save_file);
    this->controlKeysMap.emplace(KEY_I, &Uredi::quit);


    //parse argumentString
    if(!this->urediArguments.empty()){
        Argumentator argumentator(this->urediArguments);
        //path value - optional
        if(argumentator.getValues().size() > 0){
            //only one value can be provided
            if(argumentator.getValues().size() > 1){
                *(this->outStream)  << "previše putanja je uneseno. unesi argument -p za pomoć.";
                this->shouldEnd = true;
                return;
            }
            //get path value
            std::string pathValue = argumentator.getValues().at(0);
            //parse value arguments
            //  no value arguments i guess
            if(argumentator.getArguments().size() > 0){
                *(this->outStream)  << "pogrešno uneseni argumenti. unesi argument -p za pomoć.";
                this->shouldEnd = true;
                return;
            }
            //create new file on path,
            //  if there is nothing there
            if(!this->fileExists(pathValue)){                
                if(this->stozer.makeFile(pathValue) != 1){
                    *(this->outStream)  << "greška pri kreiranju novog fajla: " << pathValue << ". unesi argument -p za pomoć.";
                    this->shouldEnd = true;
                    return;
                }
            }
            //all good
            this->filePath = pathValue;
        }else{
            //parse non-value arguments
            for(const auto &arg : argumentator.getArguments()){
                if(this->argumentMap.find(arg) == this->argumentMap.end()){
                    //unknown arg
                    *(this->outStream)  << "argument '" << arg << "' nije prepoznat. unesi argument -p za pomoć.";
                    this->shouldEnd = true;
                    return;
                }else{
                    //call arg function
                    (this->*(this->argumentMap.at(arg)))();//sacuvaj boze
                }
            }
        }
    }else{
        //new unknown file
        //  do nothing i guess
    }


    //TUI
    termija::Termija &termija = termija::tra_get_instance();
    //create pane
    this->pane = this->stozer.createPane(this->PID);
    //options section entries // TODO : load from config
    this->optionsSectionEntries.push_back(std::pair("ctrl+i", " izađi"));
    this->optionsSectionEntries.push_back(std::pair("ctrl+p", " pomozi"));
        this->optionsSectionEntries.push_back(std::pair("ctrl+s", " sačuvaj"));
        this->optionsSectionEntries.push_back(std::pair("ctrl+t", " izaberi"));
        this->optionsSectionEntries.push_back(std::pair("ctrl+g", " kopiraj"));
        this->optionsSectionEntries.push_back(std::pair("ctrl+b", " nalepi"));
    //TUI widgets
    this->setup_box();
    this->setup_options_section();
    this->setup_file_name();
    this->setup_text_box();
    this->setup_cursor_position();
    this->setup_top_bar();


    //IO
        //load from file
    if(!this->filePath.empty()){
        if(!this->loadFromFile(this->filePath)){
            *(this->outStream)  << "greška pri otvaranju fajla: " + this->filePath << "\n";
            this->shouldEnd = true;
            return;
        }
    }



    //all good; set to front
    this->stozer.setFrontPID(this->PID);
}

void Uredi::cleanup(){
    //TODO: cleanup widgets
    termija::tra_remove_pane(this->pane);

}

void Uredi::draw(){
}

void Uredi::update(){
    if(shouldEnd){
        this->stozer.processTerminate(this->PID);
        return;
    }



    //Keyboard input
    std::string inputTxt = this->stozer.getInputTxt();
    KeyboardKey key = this->stozer.getPressedKey();

    if(!inputTxt.empty()){
        this->handle_text(inputTxt);
    }

    if(key != KEY_NULL){
        this->handle_command(key);
        this->handle_key(key);
    }


    //TUI
    this->update_cursor_position();
}



std::string
Uredi::getFileName(){
    if(this->filePath.empty()){
        return this->defaultFileName;
    }else{
        std::string path = this->filePath;
        string::replace_all(path, "\\", "/");
        //get file name from path
        if((path.find_last_of('/')+1) < path.size()){
            return path.substr(path.find_last_of('/')+1);
        }else{
            return path;
        }
    }
}

std::string
Uredi::getCursorPosition(){
    if(this->textBox == nullptr){
        return "";
    }

    std::pair<uint16_t, uint16_t> cursorPosition = this->textBox->getCursorPosition();
    return std::to_string(cursorPosition.second) + "," + std::to_string(cursorPosition.first);
}


void
Uredi::print_help(){
    *(this->outStream)  << "pomoc todo";
    
    this->shouldEnd = true;
}




//
//  COMMANDS
//

void
Uredi::save_file(){
    if(!this->filePath.empty()){
        //TODO: some checks here i don't know
        this->saveToFile(this->filePath);
    }else{
        //TODO: saving unknown file,
            //prompt with file name
    }

    //TODO: nemoj ovde da izlazis druze
    this->stozer.processTerminate(this->PID);
}

void
Uredi::quit(){
    //TODO:check if it has been saved before quitting
    this->stozer.processTerminate(this->PID);
}



}