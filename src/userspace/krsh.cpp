/*
    Krckov  reformirani shell 
*/

#include "krsh.h"
#include <stozer.h>
#include <termija.h>
#include <utils.h>
#include <configurator.h>

#include <plog/Log.h>
#include <regex>
#include <fstream>
#include <filesystem>

namespace stozer{


void                                move_cursor(KeyboardKey, termija::TextBox *, size_t);
void                                scroll(KeyboardKey, termija::TextBox *);
std::string                         get_command(termija::TextBox *, size_t );
void                                print_handle(termija::TextBox *, std::string, std::unordered_map<std::string, std::string> *);
uint16_t                            find_blocking_PID(Stozer &, std::map<uint16_t, bool> *);
void                                error_process_not_found(termija::TextBox *, const std::string &);
std::pair<uint16_t, bool>           run_process(Stozer &, const std::string &, const std::string &, std::map<uint16_t, bool> *, std::stringstream &);
void                                next_command(termija::TextBox *, std::string, std::unordered_map<std::string, std::string> *, size_t *, uint16_t);
void                                print_running_processes(Stozer &stozer, termija::TextBox *, std::map<uint16_t, bool> *, uint16_t);
void                                print_out_stream(termija::TextBox *, std::stringstream*);
void                                load_from_history(termija::TextBox *, std::vector<std::string> *, size_t, size_t);
void                                load_cached_command(termija::TextBox *, const std::string &, size_t);
void                                add_command_to_history(const std::string &, std::vector<std::string> *, size_t *);



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
    std::string configFilePath = this->stozer.getWorkingDirectory();
    if(filesystem::move_path(configFilePath, this->configFileRelativePath, this->stozer.getRootDirectory())){
        Configurator configurator;
        if(configurator.loadFile(configFilePath+".txt")){
            std::string value;
            value = configurator.getValue("potpis");
            if(!value.empty()){
                this->handle = value;
            }
        }
    }
    //configMap
    this->configMap.emplace("ime", "laza");
    
    //startup
    next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);



    //history
    this->load_history_file();
    historyIndex = this->historyVector.size();
}



/*
    called every frame
*/
void Krsh::update(){
    //Keyboard input
    std::string inputTxt = this->stozer.getInputTxt();
    KeyboardKey key = this->stozer.getPressedKey();
    //history
    if(key == KEY_UP){
        if(this->historyIndex > 0){
            if(this->historyIndex == this->historyVector.size())//started scrolling history, cache current unfinished command
                cachedCommand = get_command(this->textBox, this->commandStartIndex);
            this->historyIndex--;
        }
        load_from_history(this->textBox, &(this->historyVector), this->historyIndex, this->commandStartIndex);
    }
    if(key == KEY_DOWN){
        if(this->historyIndex < this->historyVector.size()-1){
            this->historyIndex++;
            load_from_history(this->textBox, &(this->historyVector), this->historyIndex, this->commandStartIndex);
        }else if(this->historyIndex == this->historyVector.size()-1){
            load_cached_command(this->textBox, this->cachedCommand, this->commandStartIndex);
            this->historyIndex = this->historyVector.size();//so that it will cache it on next UP
        }
    }
    //textBox input
    if(!inputTxt.empty() && (this->textBox->getCurrentIndex() - commandStartIndex) <= maxCommandLength){
        this->textBox->insertAtCursor(inputTxt.c_str());
        textBox->scrollToEnd();
    }
    //textBox handling
    move_cursor(key, this->textBox, this->commandStartIndex);
    scroll(key, textBox);
    if(key == KEY_DELETE)
        this->textBox->deleteAtCursor();
    if(key == KEY_BACKSPACE && this->textBox->getCurrentIndex() > this->commandStartIndex)
        this->textBox->backspaceAtCursor();



    //print outstream
    print_out_stream(this->textBox, &(this->krshOutStream));


    //REPL
    uint16_t blockingPID = find_blocking_PID(this->stozer, &(this->PIDMap));//blocking meaning it's not running in background
    if(blockingPID == 0){//no blocking processes currently running
        if(key == KEY_ENTER){
            //get and parse command
            std::string command = string::trim_string(get_command(textBox, commandStartIndex));
            add_command_to_history(command, &(this->historyVector), &(this->historyIndex));
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
                    }else if(resPair.second){//non-blocking, meaning it's runnig in background
                        print_out_stream(this->textBox, &(this->krshOutStream));
                        next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
                    }
                }
                else if(binres == -1){//exited
                    return;
                }
            }   
        }
    }else{//blocking process wait until it's done
        //process is done
        if(!stozer.isProcessRunning(blockingPID)){
            this->PIDMap.erase(blockingPID);
            next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);   
        }else{//still running
            const Process *prc = this->stozer.getRunningProcess(blockingPID);
            //command, terminate it TODO: don't terminate from h
            if(prc != nullptr && prc->isCommand() && this->PIDMap.find(blockingPID) != this->PIDMap.end()){
                this->stozer.processTerminate(blockingPID);
                this->PIDMap.erase(blockingPID);
                next_command(this->textBox, this->handle, &(this->configMap), &(this->commandStartIndex), this->maxTextBoxTextLength);
            }
        }
    }
}



void Krsh::draw(){
}




/*
    called when terminated
*/
void Krsh::cleanup(){
    //save history
    this->save_history_to_file();
    //destroy pane
    termija::tra_remove_pane(this->pane);
    //terminate processess
    std::map<uint16_t, bool>::iterator mit = PIDMap.begin();
    while(mit != PIDMap.end()){
        uint16_t PID = mit->first;
        //terminate
        this->stozer.processTerminate(PID);
        mit++;
    }
    
}


/*
    tries to run built-in commands on krsh instance,
        on success return 1, on fail 0, on exit -1
*/
int8_t Krsh::built_in_commands(const std::string &command, const std::string &arguments){
    if(command == "izađi"){
        this->stozer.processTerminate(this->PID);
        return -1;
    }
    
    if(command == "očisti"){
        this->textBox->clear();
        return 1;
    }
    
    if(command == "prc"){
        print_running_processes(this->stozer, this->textBox, &(this->PIDMap), this->getPID());
        return 1;
    }


    return 0;
}


void
Krsh::load_config_file(){
    //TODO
}


void
Krsh::load_history_file(){
    std::string historyFilePath = this->stozer.getWorkingDirectory();
    if(!filesystem::move_path(historyFilePath, this->historyFileRleativePath, this->stozer.getRootDirectory())){
        PLOG_ERROR << "paosam";
        return;
    }

    std::ifstream file(std::filesystem::u8path(historyFilePath+this->fileExtension));
    if(!file.is_open()){
        return;
    }
    std::string line;
    //skip header
    if(!std::getline(file, line) || line != "[stozer_file]"){
        file.close();
        return;
    }
    //read line by line
    size_t commandsLoaded = 0;
    while(std::getline(file, line) && commandsLoaded < this->maxHistoryLength){
        if(line.empty())
            continue;

        if(line.size() > this->maxCommandLength){
            this->historyVector.push_back(line.substr(0, this->maxCommandLength));
        }else{
            this->historyVector.push_back(line);
        }
        commandsLoaded++;
    }
    //done
    file.close();
}


void
Krsh::save_history_to_file(){
    std::string historyFilePath = this->stozer.getWorkingDirectory();
    if(!filesystem::move_path(historyFilePath, this->historyFileRleativePath, this->stozer.getRootDirectory()))
        return;

    std::ofstream file(std::filesystem::u8path(historyFilePath + this->fileExtension), std::ios::app);
    if(!file.is_open()){
        return;
    }

    //add stozer header
    // file << filesystem::FILE_HEADER << "\n";
    //save from historyVector, line by line
    for(size_t i = 0;i < this->historyVector.size();i++){
        file << this->historyVector.at(i) << "\n";
    }
    //done
    file.close();
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
        if(prc == nullptr){
            return std::pair<uint16_t, bool>(0, false);
        }
        bool isBackground = prc->isBackground();
        PIDMap->emplace(PID, isBackground);
        return std::pair<uint16_t, bool>(PID, isBackground);
    }
    else
        return std::pair<uint16_t, bool>(0, true);//true here doesn't matter, it won't be used i think
}



/*
    returns last blocking PID from map, if that thing doesn't exist returns 0
*/
uint16_t find_blocking_PID(Stozer &stozer, std::map<uint16_t, bool> *PIDMap){
    std::map<uint16_t, bool>::reverse_iterator mit = PIDMap->rbegin();
    while(mit != PIDMap->rend()){
        if(!mit->second)//blocking if isBackground is false
            return mit->first;
        mit++;
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
    error << "komanda '" << processName << "'" << " nije pronađena. proveri da li je ispravno unesena pa probaj ponovo. "
    << " iskoristi komandu 'pomozi' za listu svih dozvoljenih komandi.";
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


/*
    deletes non-finished command from textbox, inserts in it's place command at historyIndex from historyVector
*/
void load_from_history(termija::TextBox *textBox, std::vector<std::string> *historyVector, size_t historyIndex, size_t commandStartIndex){
    if(historyVector->size() == 0 || historyIndex >= historyVector->size()){
        return;
    }
    //delete non-finished command from textBox
    if(textBox->getTextLength() > commandStartIndex){//NOTE: i don't think commandStartIndex can be lower than 1 :?
        textBox->deleteAtRange(commandStartIndex-1, textBox->getTextLength());
    }
    //insert command from history
    if(!(historyVector->at(historyIndex).empty()))
        textBox->insertAtCursor(historyVector->at(historyIndex).c_str());
}

/*
    loades cached command to text box
*/
void load_cached_command(termija::TextBox *textBox, const std::string &cachedCommand, size_t commandStartIndex){
    //delete command from textBox
    if(textBox->getTextLength() > commandStartIndex){//NOTE: i don't think commandStartIndex can be lower than 1 :?
        textBox->deleteAtRange(commandStartIndex-1, textBox->getTextLength());
    }
    //insert cached command
    if(!cachedCommand.empty())
        textBox->insertAtCursor(cachedCommand.c_str());
}

/*
    adds command to history vector if last command in vector is not a duplicate
*/
void add_command_to_history(const std::string &command, std::vector<std::string> *historyVector, size_t *historyIndex){
    if(!historyVector->empty() && command == historyVector->back()){
        return;
    }
    if(!command.empty())
        historyVector->push_back(command);
    *historyIndex = historyVector->size();
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



//
//  BUILT-IN
//

/*
    prints currently running processess connected with this krsh instance
*/
void print_running_processes(Stozer &stozer, termija::TextBox *textBox, std::map<uint16_t, bool> *PIDMap, uint16_t PID){
    const Process *p;
    //add krsh info
    textBox->insertLineAtCursor(" ");//TODO: new line instead of this
    textBox->insertLineAtCursor("PID  ime");
    textBox->insertAtCursor((std::to_string(PID) + "    krsh").c_str());

    std::map<uint16_t, bool>::iterator mit = PIDMap->begin();
    while(mit != PIDMap->end()){
        uint16_t PID = mit->first;
        //get process name
        if((p = stozer.getRunningProcess(PID)) != nullptr){
            textBox->insertLineAtCursor(" ");//TODO: same
            textBox->insertAtCursor(( std::to_string(p->getPID()) + "    " + p->getName()).c_str());
        }
        mit++;
    }

}

void print_out_stream(termija::TextBox *textBox, std::stringstream *outStream){
    if(outStream->rdbuf()->in_avail() > 0){
        //new line
        textBox->insertLineAtCursor(" ");//TODO: this is not ideal,
                                    // maybe add TextBox function that adds newline flag at the end
        //print outStream
        std::vector<std::string> lines = string::split_string(outStream->str(), "\n");
        for(size_t i=0;i<lines.size();i++){
            if(i<lines.size()-1){
                textBox->insertLineAtCursor(lines.at(i).c_str());
            }else{
                //dont new line the last one, there will be new line in next_command
                textBox->insertAtCursor(lines.at(i).c_str());
            }
        
        }
        
        outStream->str(std::string());;
    }
}


}