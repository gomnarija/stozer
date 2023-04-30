#include "stozer.h"
#include <termija.h>
#include <krsh.h>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>



#include <iostream>

namespace stozer{



Stozer::Stozer(): 
ropein{nullptr},
ropeout{nullptr},
shouldEnd{false}
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
    this->pane = termija::tra_get_current_pane();
    if(this->pane == nullptr){
        PLOG_ERROR << "failed to initialize termija.";
        this->shouldEnd = true;
        return;
    }

    //ropes
    this->ropein    = rope_create_empty();
    this->ropeout   = rope_create_empty();
    if(this->ropein == nullptr || this->ropeout == nullptr){
        PLOG_ERROR << "failed to create ropes.";
        this->shouldEnd = true;
        return;
    }
}

void 
Stozer::update(){
    //termija
    if(!termija::tra_should_close()){
        termija::tra_update();
        termija::tra_draw_current();
    }else{
        this->shouldEnd = true;
    }

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
}

void 
Stozer::end(){

    //terminate running processes : TODO
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
    int currentPID = 1;
    while(this->running.find(currentPID) != this->running.end())
        currentPID++;

    return currentPID;
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
Stozer::processRun(const std::string name){
    if(this->loaded.find(name) == this->loaded.end()){
        PLOG_ERROR << "failed to find loaded process with name: " << name;
        return 0;
    }

    //clone from loaded to running vector
    std::unique_ptr<Process> processInstance((this->loaded.at(name))->instantiate());
    if(processInstance.get() == nullptr){
        PLOG_ERROR << "failed to clone loaded process with name " << name;
        return 0;
    }

    //assign PID
    uint16_t PID = get_next_PID();
    processInstance->setPID(PID);

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
        termija::tra_set_current_pane(this->pane);
    }

    return 1;
}


void 
Stozer::sout(const std::string text){
    std::cout << text;
}



}

using namespace stozer;

//start here
int main(void){
    Stozer &stz = Stozer::instance();

    stz.start();

    //load programs
    stz.processLoad(std::make_unique<Krsh>(stz));



    //run krsh
    stz.processRun("krsh");
    //stz.processLoad(std::make_unique<Krsh>(stz));
    stz.processRun("krsh");

    stz.processTerminate(2);

    while(!stz.shouldEnd){
        stz.update();
    }

    stz.end();
}