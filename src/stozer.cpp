#include "stozer.h"
#include <termija.h>
#include <utils.h>
#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>



#include <iostream>
#include <direct.h>
#include <chrono>
#include <fstream>
#include <iostream>

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
    // 09:00 AM
    this->microseconds = 0;
    this->time.hours = 9;
    this->time.minutes = 0;
    this->time.seconds = 0;
    // 02/05/1980
    this->date.days = 2;
    this->date.months = 5;
    this->date.years = 1980;

    //file system
    this->verify_file_system();
    this->rootDirectory = GetWorkingDirectory();
    filesystem::move_path(this->rootDirectory, "fs", this->getRootDirectory());

    //set user home
    this->userHomeDirectory = this->rootDirectory;
    filesystem::move_path(this->userHomeDirectory, "korisnici\\rs26", this->getRootDirectory());
    this->workingDirectory = this->userHomeDirectory;
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
        < uint16_t, std::unique_ptr<Process> >::iterator it, nit;
    for(it = running.begin(); it != running.end(); it++){
        Process *currentProcess = it->second.get();
        uint16_t PID = currentProcess->getPID();
        nit = it;nit++;//in case process gets terminated start from next one
        currentProcess->update();
        //process terminated
        if(!this->isProcessRunning(PID)){
            it = nit;
            continue;
        }
        //draw if in front
        if(currentProcess->getPID() == this->getFrontPID())
            currentProcess->draw();
    }


    //end of the frame
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    this->microseconds += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    if(this->microseconds > 1000000){//1 second passed since last time, or possibly more
        uint8_t seconds = this->microseconds / 1000000;
        this->microseconds = 0;
        this->time_forward(speedBoost * seconds);

        //reset timed keys
        this->isShifted = false;//i don't think this is used anywhere
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
    int currentPID = this->availablePID + 1;
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
bool _is_special(int key){
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

std::string _utf8chr(int cp)
{
    char c[5]={ 0x00,0x00,0x00,0x00,0x00 };
    if     (cp<=0x7F) { c[0] = cp;  }
    else if(cp<=0x7FF) { c[0] = (cp>>6)+192; c[1] = (cp&63)+128; }
    else if(0xd800<=cp && cp<=0xdfff) {} //invalid block of utf8
    else if(cp<=0xFFFF) { c[0] = (cp>>12)+224; c[1]= ((cp>>6)&63)+128; c[2]=(cp&63)+128; }
    else if(cp<=0x10FFFF) { c[0] = (cp>>18)+240; c[1] = ((cp>>12)&63)+128; c[2] = ((cp>>6)&63)+128; c[3]=(cp&63)+128; }
    return std::string(c);
}

bool _is_lat(int c){
    for(int i=0;i<LAT_KEYS_SIZE;i++){
        if(c == LAT_CHARS[i])
            return true;
    }
    return false;
}

/*
    returns pressed character
    NOTE: alphanumeric, special,  blank space
    NOTE: Doesn't pop the key queue
*/
std::string
Stozer::getInputTxt(){
    std::stringstream ss;
    int key = GetCharPressed();
    while(key > 0){
        if(isalnum(key) || _is_lat(key) || _is_special(key))
            ss << _utf8chr(key);
        key = GetCharPressed();
    }
    return ss.str();
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

bool
Stozer::isCtrl(){
    return IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
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



//file system

const std::string&
Stozer::getWorkingDirectory(){
    return this->workingDirectory;
}


const std::string&
Stozer::getRootDirectory(){
    return this->rootDirectory;
}

const std::string
Stozer::getWorkingDirectoryRelative(){
    return filesystem::relative_path(this->rootDirectory, this->workingDirectory);
}

/*
    tries to change workingDirectory to given relative path
*/
bool 
Stozer::changeWorkingDirectory(const std::string &relativePath){
    std::string newPath = this->workingDirectory;
    if(filesystem::move_path(newPath, relativePath, this->getRootDirectory()) 
            && filesystem::is_dir(newPath)
                && filesystem::is_inside(this->rootDirectory, newPath)){
        this->workingDirectory = newPath;
        return true;
    }
    return false;
}

/*
    tries to make a new directory at  given path
    1 - success, 0 - fail, -1 - no permission , -2 - invalid name
*/
int8_t 
Stozer::makeDirectory(const std::string &relativePath){
    std::string directoryPath = std::string(this->getWorkingDirectory());
    if(filesystem::move_path_wv(directoryPath, relativePath, this->getRootDirectory())){
        //check if parent directory exists and is inside user home
        std::string parentDirectory = directoryPath.substr(0, directoryPath.find_last_of(filesystem::SEPARATOR));
        std::string name = filesystem::get_name(directoryPath);
        if(!filesystem::is_valid_path(parentDirectory)){
            return 0;
        }else if(!filesystem::is_valid_name(name)){
            return -2;
        }
        else if(!filesystem::is_inside(this->userHomeDirectory, parentDirectory)){
            return -1;
        }
        //file or dir with that name already exists
        if(filesystem::is_valid_path(directoryPath)){
            return 0;
        }
        std::error_code ec; 
        return std::filesystem::create_directory(filesystem::path_to_u32(directoryPath), ec) == true;
    }else{
        return 0;
    }
}

/*
    lists the content of the given directory path
*/
std::vector<std::string>
Stozer::listDirectory(const std::string &path){
    std::vector<std::string> res;//collect entries here
    std::error_code ec;
    for (const auto & entry : std::filesystem::directory_iterator(filesystem::path_to_u32(path), ec)){
        if(ec.value() != 0)
            continue; 
        std::string resEntry;
        std::string pathStr = entry.path().string();
        //file or dir
        if(filesystem::is_txt(pathStr)){
            std::string name = pathStr.substr(pathStr.find_last_of(filesystem::SEPARATOR) + 1);
            name = name.substr(0, name.length()-4);
            resEntry += "f";
            resEntry += "    ";
            resEntry += name;
        }else if(entry.is_directory()){
            std::string name = pathStr.substr(pathStr.find_last_of(filesystem::SEPARATOR) + 1);
            resEntry += "d";
            resEntry += "    ";
            resEntry += name;
        }else{
            continue;
        }
        res.push_back(resEntry);
    }
    return res;
}

/*
    tries to remove the directory or file at  given path
    1 - success, 0 - fail, -1 - no permission, -2 - non-empty dir ( not forced )
*/
int8_t 
Stozer::removeFileOrDir(const std::string &relativePath, bool isForced){
    std::string dofPath = std::string(this->getWorkingDirectory());
    //check if there is dir of file at path
    if(filesystem::move_path_wv(dofPath, relativePath, this->getRootDirectory()) && filesystem::is_valid_path(dofPath)){
        std::string parentDirectory = dofPath.substr(0, dofPath.find_last_of(filesystem::SEPARATOR));
        //parent directory must exist and be inside user home
        if(!filesystem::is_valid_path(parentDirectory))
            return 0;
        else if(!filesystem::is_inside(this->userHomeDirectory, parentDirectory))
            return -1;
        //file or dir
        if(!filesystem::is_dir(dofPath)){
            dofPath+=".txt";//file, add extension
        }else{
            //dir, must be empty
            if(!filesystem::is_dir_empty(dofPath) && !isForced){
                return -2;
            }
        }
        //remove
        std::error_code ec;
        std::filesystem::remove_all(filesystem::path_to_u32(dofPath), ec);
        return  ec.value() == 0;
    }else{
        return 0;
    }
}


/*
    tries to make a new .txt at given path
    1 - success, 0 - fail, -1 - no permission, -2 - invalid name
*/
int8_t
Stozer::makeFile(const std::string &relativePath){
    std::string filePath = std::string(this->getWorkingDirectory());
    if(filesystem::move_path_wv(filePath, relativePath, this->getRootDirectory())){
        std::string parentDirectory = filePath.substr(0, filePath.find_last_of(filesystem::SEPARATOR));
        std::string name = filesystem::get_name(filePath);
        //check if parent directory exists and is inside user home
        if(!filesystem::is_valid_path(parentDirectory)){
            return 0;
        }else if(!filesystem::is_valid_name(name)){
            return -2;
        }
        else if(!filesystem::is_inside(this->userHomeDirectory, parentDirectory)){
            return -1;
        }
        
        //file or dir with that name already exists
        if(filesystem::is_valid_path(filePath))
            return 0;
        filePath += ".txt";//add extension
        std::ofstream ofs(std::filesystem::u8path(filePath));
        if(!ofs.is_open())
            return 0;
        //add header
        ofs << filesystem::FILE_HEADER << "\n"; 
        ofs.close();
        return 1;
    }else{
        return 0;
    }
}

/*
    tries to move/rename file or dir
    1 - success, 0 - fail, -1 - no permission, -2 - invalid name 
*/
int8_t
Stozer::moveFileOrDir(const std::string &relativePath, const std::string &newRelativePath){
    std::string dofPath = std::string(this->getWorkingDirectory());
    std::string newDofPath = std::string(this->getWorkingDirectory());

    //old path must exist
    if(filesystem::move_path_wv(dofPath, relativePath, this->getRootDirectory()) && filesystem::is_valid_path(dofPath) 
        && filesystem::move_path_wv(newDofPath, newRelativePath, this->getRootDirectory())){
            
        std::string parentDirectory = dofPath.substr(0, dofPath.find_last_of(filesystem::SEPARATOR));
        std::string newParentDirectory = newDofPath.substr(0, newDofPath.find_last_of(filesystem::SEPARATOR));
        std::string name = filesystem::get_name(newDofPath);
        if(!filesystem::is_valid_name(name)){
            return -2;
        }

        //dir or path
        if(!filesystem::is_dir(dofPath)){
            //old is file, add extension
            dofPath+=".txt";
            //if there is something at new path, it must be dir
            // if it is dir, place it inside
            if(filesystem::is_dir(newDofPath)){
                if(newDofPath.at(newDofPath.length()-1) != filesystem::SEPARATOR)
                    newDofPath.push_back(filesystem::SEPARATOR);
                newParentDirectory = newDofPath;//new parent directory becomes existing dir at given path
                newDofPath += dofPath.substr(dofPath.find_last_of(filesystem::SEPARATOR) + 1);
                //check if there is file with that name in the dir
                if(filesystem::is_valid_path(newDofPath))
                    return 0;
            }else if(filesystem::is_valid_path(newDofPath)){
                //there is something at new path that is not dir, this is not allowed
                return 0;
            }else{
                //new is file, add extension
                newDofPath+=".txt";
            }
        }else{
            //dir
            if(filesystem::is_dir(newDofPath)){
                if(newDofPath.at(newDofPath.length()-1) != filesystem::SEPARATOR)
                    newDofPath.push_back(filesystem::SEPARATOR);
                newParentDirectory = newDofPath;//new parent directory becomes existing dir at given path
                newDofPath += dofPath.substr(dofPath.find_last_of(filesystem::SEPARATOR) + 1);
                //check if there is file with that name in the dir
                if(filesystem::is_valid_path(newDofPath))
                    return 0;
            }else if(filesystem::is_valid_path(newDofPath)){
                //there is something at new path that is not dir, this is not allowed
                return 0;
            }
        }
        //both parent directories should exist and be inside user home
        if(!filesystem::is_valid_path(parentDirectory) || !filesystem::is_valid_path(newParentDirectory))
            return 0;
        else if(!filesystem::is_inside(this->userHomeDirectory, parentDirectory) || 
                    !filesystem::is_inside(this->userHomeDirectory, newParentDirectory)){
            return -1;
        }
        //rename
        std::error_code ec; 
        std::filesystem::rename(filesystem::path_to_u32(dofPath), filesystem::path_to_u32(newDofPath), ec);
        return ec.value() == 0;
    }else{
        return 0;
    }
}

/*
    tries to copy file or dir
    1 - success, 0 - fail, -1 - no permission 
*/
int8_t
Stozer::copyFileOrDir(const std::string &relativePath, const std::string &newRelativePath, std::filesystem::copy_options copyOptions){
    std::string dofPath = std::string(this->getWorkingDirectory());
    std::string newDofPath = std::string(this->getWorkingDirectory());

    //old path must exist
    if(filesystem::move_path_wv(dofPath, relativePath, this->getRootDirectory()) && filesystem::is_valid_path(dofPath) 
        && filesystem::move_path_wv(newDofPath, newRelativePath, this->getRootDirectory())){
            
        std::string parentDirectory = dofPath.substr(0, dofPath.find_last_of(filesystem::SEPARATOR));
        std::string newParentDirectory = newDofPath.substr(0, newDofPath.find_last_of(filesystem::SEPARATOR));
        std::string name = filesystem::get_name(newDofPath);
        if(!filesystem::is_valid_name(name)){
            return -2;
        }

        //dir or path
        if(!filesystem::is_dir(dofPath)){
            //old is file, add extension
            dofPath+=".txt";
            //if there is something at new path, it must be dir
            // if it is dir, place it inside
            if(filesystem::is_dir(newDofPath)){
                if(newDofPath.at(newDofPath.length()-1) != filesystem::SEPARATOR)
                    newDofPath.push_back(filesystem::SEPARATOR);
                newParentDirectory = newDofPath;//new parent directory becomes existing dir at given path
                newDofPath += dofPath.substr(dofPath.find_last_of(filesystem::SEPARATOR) + 1);
                //check if there is file with that name in the dir
                if(filesystem::is_valid_path(newDofPath))
                    return 0;
            }else if(filesystem::is_valid_path(newDofPath)){
                //there is something at new path that is not dir, this is not allowed
                return 0;
            }else{
                //new is file, add extension
                newDofPath+=".txt";
            }
        }else{
            //dir
            if(filesystem::is_dir(newDofPath)){
                if(newDofPath.at(newDofPath.length()-1) != filesystem::SEPARATOR)
                    newDofPath.push_back(filesystem::SEPARATOR);
                newParentDirectory = newDofPath;//new parent directory becomes existing dir at given path
                newDofPath += dofPath.substr(dofPath.find_last_of(filesystem::SEPARATOR) + 1);
                //check if there is file with that name in the dir
                if(filesystem::is_valid_path(newDofPath))
                    return 0;
            }else if(filesystem::is_valid_path(newDofPath)){
                //there is something at new path that is not dir, this is not allowed
                return 0;
            }
        }
        //both parent directories should exist and be inside user home
        if(!filesystem::is_valid_path(parentDirectory) || !filesystem::is_valid_path(newParentDirectory))
            return 0;
        else if(!filesystem::is_inside(this->userHomeDirectory, parentDirectory) || 
                    !filesystem::is_inside(this->userHomeDirectory, newParentDirectory)){
            return -1;
        }
        //copy
        std::error_code ec; 
        std::filesystem::copy(filesystem::path_to_u32(dofPath), filesystem::path_to_u32(newDofPath), copyOptions, ec);
        return ec.value() == 0;
    }else{
        return 0;
    }
}



/*
    verify that all needed directories and files are present
*/
void 
Stozer::verify_file_system(){
    for(int i=0;i<REQUIRED_PATHS.size();i++){
        std::string path = GetWorkingDirectory();
        filesystem::move_path(path, REQUIRED_PATHS[i], this->getRootDirectory());//tries to move to given path,
        if(path == GetWorkingDirectory()){//                    if it fails it will stay at GetWorkingDirectory()
            PLOG_ERROR << "required path " + REQUIRED_PATHS[i] + " doesn't exist, aborting.";
            this->shouldEnd = true;
        }
    }
}

}

