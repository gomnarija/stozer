/*
    Krckov  reformirani shell 
*/

#include "krsh.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>

#include <plog/Log.h>
#include <regex>

namespace stozer{


void                                move_cursor(KeyboardKey, termija::TextBox *, size_t);
void                                scroll(KeyboardKey, termija::TextBox *);
std::string                         get_command(termija::TextBox *, size_t );
void                                print_handle(termija::TextBox *, std::string, std::unordered_map<std::string, std::string> *);
uint16_t                            find_blocking_PID(Stozer &, std::map<uint16_t, bool> *);
void                                error_process_not_found(termija::TextBox *, const std::string &);
std::pair<uint16_t, bool>           run_process(Stozer &, const std::string &, const std::string &, std::map<uint16_t, bool> *, std::stringstream &);
void                                next_command(termija::TextBox *, std::string, std::unordered_map<std::string, std::string> *, size_t *, uint16_t);

Krsh::Krsh(Stozer &stozer, const std::string &arguments) : Process(stozer, arguments){

    this->name              = "krsh";//unique
    
}


Process* Krsh::instantiate(const std::string &arguments){
    return new Krsh(this->stozer, arguments);
}




/*
    called when started
*/
void Krsh::setup(){
    termija::Termija &termija = termija::tra_get_instance();

    //drawn process; set to front
    this->stozer.setFrontPID(this->PID);

    //create pane
    this->pane = this->stozer.createPane(this->PID);


    //create widgets
        //textBox
            //position
            uint16_t x = ((float)this->pane->width / 100) * this->textBoxMargin;
            uint16_t y = ((float)this->pane->height / 100) * this->textBoxMargin;
            //size
            uint16_t width  = std::max((this->pane->width - (x * 2)) / (termija.fontWidth+termija.fontSpacing), 1);
            uint16_t height = std::max((this->pane->height - (y * 2)) / (termija.fontHeight+termija.fontSpacing), 1);

        this->textBox = (termija::TextBox*)termija::tra_add_widget(*(this->pane),
                                                std::make_unique<termija::TextBox>(x, y, width, height));


    //config TODO
    this->configMap.emplace("ime", "laza");
    this->handle = "%ime@stozer>";

    //startup
    next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);

}



/*
    called every frame
*/
void Krsh::update(){

    //Keyboard input
    char pressedChar = this->stozer.getPressedChar();
    KeyboardKey key = this->stozer.getPressedKey();

    //case
    if(key == KEY_CAPS_LOCK)
        stozer.caseToggle();

    //textBox input
    if(pressedChar > 0 && (this->textBox->getCurrentIndex() - commandStartIndex) <= maxCommandLength){
        this->textBox->insertAtCursor(std::string(1, pressedChar).c_str());
        textBox->scrollToEnd();
    }

    //textBox handling
    move_cursor(key, this->textBox, this->commandStartIndex);
    scroll(key, textBox);

    if(key == KEY_DELETE)
        this->textBox->deleteAtCursor();
    if(key == KEY_BACKSPACE && this->textBox->getCurrentIndex() > this->commandStartIndex)
        this->textBox->backspaceAtCursor();


    //REPL
    uint16_t blockingPID = find_blocking_PID(this->stozer, &(this->PIDMap));
    if(blockingPID == 0){//no blocking processes currently running
        if(key == KEY_ENTER){
            //get and parse command
            std::string command = string::trim_string(get_command(textBox, commandStartIndex));
            if(command.empty()){
                next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
            }else{
                std::vector<std::string> parsed = string::split_string(command, " ");
                std::string processName = parsed.size() > 0 ? parsed.at(0) : "";
                std::string arguments = string::trim_string(string::remove_extra_whitespace(string::split_string_into_two(command, " ").second));
                //try built-in
                int8_t binres = built_in_commands(processName, arguments);
                if(binres == 1){
                    next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
                }
                else if(binres == 0){
                    //else try to run as process
                    std::pair<uint16_t, bool> resPair = run_process(this->stozer, processName, arguments, &(this->PIDMap), this->krshOutStream);
                    if(resPair.first == 0){//no process with that name
                        error_process_not_found(textBox, processName);
                        next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
                    }else if(resPair.second){//non-blocking
                        next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
                    }
                }
                else if(binres == -1){//exited
                    return;
                }
            }   
        }
    }else{//blocking processwait until it's done
        //if blockingPID is still running, print out and wait
        // else print_handle and move commandStartIndex
        if(stozer.isProcessRunning(blockingPID) && this->krshOutStream.rdbuf()->in_avail() > 0){
            //new line
            this->textBox->insertLineAtCursor(" ");//TODO: this is not ideal,
                                        // maybe add TextBox function that adds newline flag at the end
            //print outStream
            std::vector<std::string> lines = string::split_string(this->krshOutStream.str(), "\n");
            for(size_t i=0;i<lines.size();i++){
                if(i<lines.size()-1){
                    this->textBox->insertLineAtCursor(lines.at(i).c_str());
                }else{
                    //dont new line the last one, there will be new line in next_command
                    this->textBox->insertAtCursor(lines.at(i).c_str());
                }
            
            }
            
            this->krshOutStream.str(std::string());;
        }else{//process is done
            this->PIDMap.erase(blockingPID);
            next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
        }

        const Process *prc = this->stozer.getRunningProcess(blockingPID);
        //command, terminate it
        if(prc != nullptr && prc->isCommand() && this->PIDMap.find(blockingPID) != this->PIDMap.end()){
            this->stozer.processTerminate(blockingPID);
            this->PIDMap.erase(blockingPID);
            next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
        }
        
    }


}



void Krsh::draw(){
    this->textBox->update();
}




/*
    called when terminated
*/
void Krsh::cleanup(){
    //destroy pane
    termija::tra_remove_pane(this->pane);
    
}


/*
    tries to run built-in commands on krsh instance,
        on success return 1, on fail 0, on exit -1
*/
int8_t Krsh::built_in_commands(const std::string &command, const std::string &arguments){
    if(command == "izadji"){
        this->stozer.processTerminate(this->PID);
        return -1;
    }
    
    if(command == "ocisti"){
        this->textBox->clear();
        return 1;
    }
    



    return 0;
}


//
//  REPL
//


/*
    tries to run process with given name,
        on fail returns 0 on success PID
*/
std::pair<uint16_t, bool> run_process(Stozer &stozer, const std::string &processName, 
        const std::string &arguments, std::map<uint16_t, bool> *PIDMap, std::stringstream &outStream){
    if(!stozer.isProcessLoaded(processName)){
        return std::pair<uint16_t, bool>(0, false);
    }
    std::pair<uint16_t, bool> result;
    uint16_t PID = stozer.processRun(processName, arguments, outStream);
    if(PID > 0){
        const Process *prc = stozer.getRunningProcess(PID);
        if(prc == nullptr)
            return std::pair<uint16_t, bool>(0, false);

        bool isBackground = prc->isBackground();
        PIDMap->emplace(PID, isBackground);
        return std::pair<uint16_t, bool>(PID, isBackground);
    }
    else
        return std::pair<uint16_t, bool>(0, true);
}



/*
    returns last blocking PID from map, if that thing doesn't exist returns 0
*/
uint16_t find_blocking_PID(Stozer &stozer, std::map<uint16_t, bool> *PIDMap){
    std::map<uint16_t, bool>::reverse_iterator mit = PIDMap->rbegin();
    while(mit != PIDMap->rend()){
        if(!mit->second)//blocking if isBackground is false
            return mit->first;
        mit--;
    }

    return 0;
}



/*
    returns currently entered command from textBox
*/
std::string get_command(termija::TextBox *textBox, size_t commandStartIndex){
    return textBox->getText(commandStartIndex, textBox->getTextLength());
}

/*
    print error
*/
void error_process_not_found(termija::TextBox *textBox, const std::string &processName){
    //move cursor to the end
    textBox->setCursorIndex(textBox->getTextLength());

    //new line
    textBox->insertLineAtCursor(" ");//TODO: this is not ideal,
                                        // maybe add TextBox function that adds newline flag at the end

    std::stringstream error;
    error << "Komanda '" << processName << "'" << " nije pronadjena. Proveri da li je ispravno unesena pa probaj ponovo. "
    << " Iskoristi komandu 'pomozi' za listu svih dozvoljenih komandi.";
    textBox->insertAtCursor(error.str().c_str());

}




/*
    print handle in the new line before start of the next command
*/
void print_handle(termija::TextBox *textBox, std::string handle, 
        std::unordered_map<std::string, std::string> *configMap){
    
    //move cursor to the end
    if(textBox->getTextLength() > 0)
        textBox->setCursorIndex(textBox->getTextLength());

    //new line
    if(textBox->getTextLength() > 0)
        textBox->insertLineAtCursor(" ");//TODO: this is not ideal,
                                        // maybe add TextBox function that adds newline flag at the end

    //handle fields:
    //go trough configMap, trying to find every config field name with % prefix in handle,
    // if found, replace it with config field value
    std::unordered_map<std::string, std::string>::iterator mit = configMap->begin();
    while(mit != configMap->end()){
        std::string regex = "\\%" + mit->first;
        handle = std::regex_replace(handle, std::regex(regex), mit->second);
        mit++;
    }

    textBox->insertAtCursor(handle.c_str());
}

void next_command(termija::TextBox *textBox, std::string handle, std::unordered_map<std::string, std::string> *configMap, size_t *commandStartIndex, uint16_t maxTextBoxTextLength){
    if(textBox->getTextLength() > maxTextBoxTextLength)
        textBox->clear();
    
    print_handle(textBox, handle, configMap);
    *commandStartIndex = textBox->getCurrentIndex();
    textBox->scrollToEnd();
}


//
//  TEXTBOX HANDLING
//

/*
    move textBox cursor on key input
*/
void move_cursor(KeyboardKey key, termija::TextBox *textBox, size_t commandStartIndex){
    if(key == KEY_LEFT && textBox->getCurrentIndex() > commandStartIndex){//move left only if over command start index
        textBox->cursorWalkLeft(1);
    }
    if(key == KEY_RIGHT){
        textBox->cursorWalkRight(1);
    }
}

/*
    scroll textBox on key input
*/
void scroll(KeyboardKey key, termija::TextBox *textBox){
    if(key == KEY_PAGE_DOWN){
        textBox->frameCursorMove(1);
    }
    else if(key == KEY_PAGE_UP){
        textBox->frameCursorMove(-1);
    }
}


}