#include "stozer.h"
#include <termija.h>
#include <utils.h>
#include <krsh.h>
#include <sat.h>
#include <kalendar.h>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>



#include <iostream>
#include <chrono>

namespace stozer{



Stozer::Stozer(): 
shouldEnd{false},
isUpperCase(false),
availablePID{1}
{}


/*
    returns frontPid,
        if none set returns 0
*/
uint16_t
Stozer::getFrontPID(){
    if(this->frontStack.empty()){
        return 0;
    }else{
        return this->frontStack.top();
    }
}


/*
    sets the given PID as frontPID
*/
void
Stozer::setFrontPID(uint16_t PID){
    if(this->running.find(PID) == this->running.end()){
        PLOG_ERROR << "couldn't find running process with PID: " << PID << " ,aborted."; 
    }else{
        this->frontStack.push(PID);
    }
}


void
Stozer::caseToggle(){
    this->isUpperCase = !this->isUpperCase;
}

void
Stozer::setUpperCase(){
    this->isUpperCase = true;
}

void
Stozer::setLowerCase(){
    this->isUpperCase = false;
}


//
//  CYCLE
//



void 
Stozer::start(){
    //plog
    plog::init(plog::debug, "stozer.log");

    //termija
    termija::tra_load_config("termija.conf");
    termija::tra_init_termija();
    termija::tra_set_fps(60);
    this->default_pane = termija::tra_get_current_pane();
    if(this->default_pane == nullptr){
        PLOG_ERROR << "failed to initialize termija.";
        this->shouldEnd = true;
        return;
    }

    //date time
    this->microseconds = 0;
    this->time.hours = 9;
    this->time.minutes = 0;
    this->time.seconds = 0;

    this->date.days = 2;
    this->date.months = 5;
    this->date.years = 1980;


    date_forward(2783);

}

void 
Stozer::update(){
    //start of the frame
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    //log out
    if(this->logStream.rdbuf()->in_avail() > 0){
        PLOG_DEBUG << this->logStream.str();
        this->logStream.str(std::string());
    }

    //termija
    if(!termija::tra_should_close()){
        termija::tra_update();
        termija::tra_draw_current();
    }else{
        this->shouldEnd = true;
    }

    //input
    int key = GetKeyPressed();
    if(key != 0)
        this->pressKey((KeyboardKey) key);
    isShifted = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);


    //processes
    std::unordered_map
        < uint16_t, std::unique_ptr<Process> >::iterator it;
    for(it = running.begin(); it != running.end(); it++){
        Process *currentProcess = it->second.get();
        currentProcess->update();
        //draw if in front
        if(currentProcess->getPID() == this->getFrontPID())
            currentProcess->draw();
    }


    //end of the frame
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    this->microseconds += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    if(this->microseconds > 1000000){//1 second passed since last time
        this->microseconds = 0;
        this->time_forward(speedBoost);
    }
}

void 
Stozer::end(){

    //terminate running processes
    std::unordered_map
        < uint16_t, std::unique_ptr<Process> >::iterator it;
    for(it = running.begin(); it != running.end(); it++){
        this->processTerminate(it->first);
    }

    //termija
    termija::tra_terminate();

}


//
//  UTILS
//


/*
    returns the first avaliable PID
*/
uint16_t 
Stozer::get_next_PID(){
    int currentPID = this->availablePID;
    while(this->running.find(currentPID) != this->running.end())
        currentPID++;

    return this->availablePID=currentPID;
}



//
//  PROCESS MANIPULATION
//

/*
    loads the given process into map; consumes pointer;
        returns:
            success - 1
            process with that name already exists - 0
            error - -1 
*/
int8_t 
Stozer::processLoad(std::unique_ptr<Process> process){
    if(process->getName().empty()){
        PLOG_ERROR << "process name must not be empty, aborted.";
        return -1;
    }

    //insert into map
    this->loaded.insert(std::make_pair(process->getName(), std::move(process)));
    return 1;
}


/*
    runs the loaded process with the given name,
        on success returns PID, on error 0
*/
uint16_t 
Stozer::processRun(const std::string &name, const std::string &arguments, std::stringstream &outStream){
    if(this->loaded.find(name) == this->loaded.end()){
        PLOG_ERROR << "failed to find loaded process with name: " << name;
        return 0;
    }

    //clone from loaded to running vector
    std::unique_ptr<Process> processInstance((this->loaded.at(name))->instantiate(arguments));
    if(processInstance.get() == nullptr){
        PLOG_ERROR << "failed to clone loaded process with name " << name;
        return 0;
    }

    //assign PID
    uint16_t PID = get_next_PID();
    processInstance->setPID(PID);

    //set outStream
    processInstance->setOutStream(&outStream);

    //push to vector
    Process *p = processInstance.get();
    this->running.insert(std::make_pair(PID, std::move(processInstance)));

    //run process setup
    p->setup();
    
    return PID;
}


/*
    terminates the process with given PID,
        on error returns -1
*/
int8_t
Stozer::processTerminate(uint16_t PID){
    if(this->running.find(PID) == this->running.end()){
        PLOG_ERROR << "couldn't find running process with PID: " << PID << " ,aborted."; 
        return -1;
    }

    //call process cleanup
    this->running.at(PID)->cleanup();

    //remove from running map
    this->running.erase(PID);

    //move from front
    if(this->getFrontPID() == PID){
        this->frontStack.pop();
    }

    //remove from pane map
    this->frontPaneMap.erase(PID);

    //set next current pane, or default
    if(this->getFrontPID() > 0 && frontPaneMap.find(this->getFrontPID()) != frontPaneMap.end()){
        termija::tra_set_current_pane(frontPaneMap.at(this->getFrontPID()));
    }else{
        termija::tra_set_current_pane(this->default_pane);
    }

    return 1;
}


/*
    checks if process with given name is loaded
*/
bool
Stozer::isProcessLoaded(const std::string &name){
    return this->loaded.find(name) != this->loaded.end();
}

/*
    checks if process with given PID is running
*/
bool
Stozer::isProcessRunning(uint16_t PID){
    return this->running.find(PID) != this->running.end();
}


/*
    returns currently running process,
        on error returns nullptr
*/
const Process*
Stozer::getRunningProcess(uint16_t PID){
    if(!(this->isProcessRunning(PID))){
        PLOG_ERROR << "no process with that PID is currently running, aborted. PID: " << PID;
        return nullptr;
    }

    return this->running.at(PID).get();
}



//
// TERMIJA
//


/*
    creates new window-sized panes, connects it with the givne PID and imposes it as current pane
*/
termija::Pane*
Stozer::createPane(uint16_t PID){
    termija::Pane *pane = termija::tra_impose_duplicate_pane(this->default_pane);
    this->frontPaneMap.insert(std::make_pair(PID, pane));
    return pane;
}

//
//  KEYBOARD
//

/*
    helper, checks if given key is special key ( as defined in stozer.h )
*/
bool _is_special(KeyboardKey key){
    for(int i=0;i<SPECIAL_KEYS_SIZE;i++){
        if(key == SPECIAL_KEYS[i])
            return true;
    }
    return false;
}

/*
    helper, checks if given key is special char ( as defined in stozer.h )
*/
bool _is_special(char key){
    for(int i=0;i<SPECIAL_KEYS_SIZE;i++){
        if(key == SPECIAL_CHARS[i])
            return true;
    }
    return false;
}

/*
    helper, converts upppercase char to lowercase
*/
char _to_lowercase(char c){
    return c + 32;
}

/*
    tries changing given character to alternative one ( as defined in stozer.h ),
        on fail returns given character
*/
char _shift(char c){
    if(SHIFTED.find(c) != SHIFTED.end())
        return SHIFTED.at(c);

    return c;
}


/*
    returns front of the key buffer,
        on empty buffer returns KEY_NULL;

*/
KeyboardKey
Stozer::getPressedKey(){
    if(this->keyQueue.empty())
        return KEY_NULL;

    KeyboardKey key = this->keyQueue.front();
    this->keyQueue.pop();
    return key;
}

/*
    tries to convert front of the key buffer to a character,
        on fail returns 0;

    NOTE: alphanumeric, blank space
    NOTE: Doesn't pop the key queue
*/
char
Stozer::getPressedChar(){
    if(this->keyQueue.empty())
        return 0;

    KeyboardKey key = this->keyQueue.front();
    char c = (char)key;
    char res = 0;

    //togle uppercase if shifted
    isUpperCase = isShifted ? !isUpperCase : isUpperCase;
    
    if(std::isalnum(c) || _is_special(c)){
        if(!isUpperCase && std::isalpha(key))
            res =  _to_lowercase(key);
        else if(!std::isalpha(key))
            res = isShifted ? _shift((char)key) : key;
        else
            res = key;
    }

    //toggle upercase back
    isUpperCase = isShifted ? !isUpperCase : isUpperCase;
    return res;
}



/*
    clears key buffer
*/
void
Stozer::clearKeys(){
    std::queue<KeyboardKey> empty;
    std::swap(this->keyQueue, empty);
}

/*
    places the given key into the key buffer,
        only alphanumeric and special keys allowed
*/
void
Stozer::pressKey(const KeyboardKey key){
    if(std::isalnum(key) || _is_special(key))
        this->keyQueue.push(key);
} 



// date time
const Time&
Stozer::getTime(){
    return this->time;
}

const Date&
Stozer::getDate(){
    return this->date;
}

/*
    moves time forward given number of seconds
*/
void 
Stozer::time_forward(uint16_t passed){

    passed = this->time.seconds + passed;
    this->time.seconds = passed % 60;
    
    passed = (this->time.minutes + (passed / 60));
    this->time.minutes = passed % 60;

    passed = (this->time.hours + (passed / 60));
    this->time.hours = passed % 24;

}

/*
    moves date forward given number of days
*/
void 
Stozer::date_forward(uint16_t passed){
    do{
        uint8_t daysInMonth = datetime::days_in_a_month(this->date);
        if((date.days + passed) <= daysInMonth){
            date.days += passed;
            passed = 0;
        }else{
            if((daysInMonth - date.days + 1) < passed)
                passed -= daysInMonth - date.days + 1;
            else
                passed = 0;

            date.days = 1;
            if(date.months < 12){
                date.months ++;
            }else{
                date.months = 1;
                date.years++;
            }
        } 
    }while(passed > 0);
}






}

using namespace stozer;

//start here
int main(void){
    Stozer &stz = Stozer::instance();

    stz.start();

    //load programs
    stz.processLoad(std::make_unique<Krsh>(stz, ""));
    stz.processLoad(std::make_unique<Sat>(stz, ""));
    stz.processLoad(std::make_unique<Kalendar>(stz, ""));


    //run krsh
    stz.processRun("krsh", "", stz.logStream);



    while(!stz.shouldEnd){
         stz.update();
    }

    PLOG_DEBUG << "muda";

    stz.end();



    return 0;
}