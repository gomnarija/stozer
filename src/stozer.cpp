#include "stozer.h"
#include <termija.h>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>


#include <iostream>

namespace stozer{



Stozer::Stozer(): 
pane{nullptr},
ropein{nullptr},
ropeout{nullptr},
shouldEnd{false},
activePID{0},
active{nullptr}
{}



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
    if(this->pane == nullptr){//termija not initialized properly
        PLOG_ERROR << "failed to initialize termija.";
        this->shouldEnd = true;
        return;
    }

    //ropes
    this->ropein    = rope_create_empty();
    this->ropeout   = rope_create_empty();
    if(this->ropein == nullptr || this->ropeout == nullptr){
        PLOG_ERROR << "failed create ropes.";
        this->shouldEnd = true;
        return;
    }
}

void 
Stozer::update(){
    //termija
    if(!termija::tra_should_close()){
        termija::tra_update();
        termija::tra_draw();
    }else{
        this->shouldEnd = true;
    }

    //process'
    for(int i=0;i<this->running.size();i++){
        Process *p = this->running.at(i).get();
        p->update();
    }
}

void 
Stozer::end(){


    //termija
    termija::tra_terminate();

}





//
//  UTILS
//


/*
    returns the next avaliable PID
*/
uint16_t 
Stozer::get_next_PID(){
    int currentPID = 1;
    while(find_running_by_PID(currentPID)!=-1)
        currentPID++;

    return currentPID;
} 


/*
    returns index for a loaded process with a given name,
        on error or nothing found returns -1
*/
size_t 
Stozer::find_loaded_by_name(const std::string name){
    for(int i=0;i<this->loaded.size();i++){
        Process *p = this->loaded.at(i).get();
        if(p->getName() == name){
            return i;
        }
    }
    return -1;
}


/*
    returns index for a running process with a given PID,
        on error or nothing found returns -1
*/
size_t 
Stozer::find_running_by_PID(const uint8_t PID){
    for(int i=0;i<this->running.size();i++){
        Process *p = this->running.at(i).get();
        if(p->getPID() == PID){
            return i;
        }
    }
    return -1;
}

//
//  PROCESS MANIPULATION
//

/*
    loads the given process into vector; consumes pointer;
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

    //check if process with that name already exists
    if(find_loaded_by_name(process->getName()) != -1){
            PLOG_WARNING << "process with name: " << process->getName() << " already exists."; 
            return 0;
    }

    //insert into vector
    this->loaded.push_back(std::move(process));
    return 1;
}


/*
    runs the loaded process with the given name,
        on success returns PID, on error -1
*/
int8_t 
Stozer::processRun(const std::string name){
    //find loaded vector index with that name
    size_t processIndex = find_loaded_by_name(name);
    if(processIndex == -1 || processIndex >= this->loaded.size()){
        PLOG_ERROR << "failed to find loaded process with name: " << name;
        return -1;
    }

    //add from loaded to running vector, assign PID
    uint16_t PID = get_next_PID();
    this->loaded.at(processIndex)->setPID(PID);
    this->running.push_back(std::move(this->loaded.at(processIndex)));

    return PID;
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



    while(!stz.shouldEnd){
        stz.update();
    }

    stz.end();
}