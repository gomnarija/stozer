#ifndef STOZER_H
#define STOZER_H


#include <termija.h>
#include <raylib.h>

#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <string>
#include <sstream>
#include <memory>
#include <filesystem>


//forward declaration of the main function in global namespace
int main(void);

namespace stozer{


class Stozer;


//process
class Process{
protected:
    Process(Stozer &stozer, const std::string &);

    std::string                     name;//unique
    uint16_t                        PID=0;//0 default,starts from 1
    bool                            isBackgroundProcess=false;//is process running in the background, not by default
    bool                            isCommandProcess=false;//terminates after setup
    std::stringstream               *outStream;

public:
    Stozer                         &stozer;
    
    const std::string &             getName() const;
    uint16_t                        getPID() const;
    void                            setPID(uint16_t);
    void                            setOutStream(std::stringstream *);
    bool                            isBackground() const;
    bool                            isCommand() const;

    //cycle
    virtual void setup()=0;
    virtual void update()=0;
    virtual void cleanup()=0;
    virtual void draw()=0;


    virtual Process*                instantiate(const std::string &) =0;
};


struct Time{
    uint8_t hours, minutes, seconds;
};

struct Date{
    uint16_t years, months, days;
};

//singleton
class Stozer{
    Stozer();
    Stozer(const Stozer&)           = delete;

private:
    //termija
    termija::Pane                                              *default_pane;


    std::unordered_map
        < std::string, std::unique_ptr<Process> >               loaded;
    std::unordered_map
        < uint16_t, std::unique_ptr<Process> >                  running;

    //currently active/drawn Processes
    std::stack<uint16_t>                                        frontStack;
    std::unordered_map
        < uint16_t, termija::Pane* >                            frontPaneMap;

    uint16_t                                                    availablePID;

    //keyboard
    std::queue<KeyboardKey>                                     keyQueue;
    
    bool                                                        isUpperCase;
    bool                                                        isShifted=false;



    //date time
    uint64_t                                                    microseconds;
    uint8_t                                                     speedBoost=1;

    Time                                                        time;
    Date                                                        date;


    //file system
    std::string                                                 rootDirectory;
    std::string                                                 userHomeDirectory;
    std::string                                                 workingDirectory;


    //cycle
    void            start();
    void            update();
    void            end();

    //util
    uint16_t        get_next_PID();
    void            time_forward(uint16_t);
    void            date_forward(uint16_t);
    void            verify_file_system();



public:
    bool                                                         shouldEnd;

    //text buffers
    std::stringstream                                           logStream;


    uint16_t                                                     getFrontPID();
    void                                                         setFrontPID(uint16_t);

    //process operations
    int8_t           processLoad(std::unique_ptr<Process>);
    uint16_t         processRun(const std::string&, const std::string &arguments, std::stringstream&);
    int8_t           processTerminate(uint16_t);
    bool             isProcessLoaded(const std::string&);
    bool             isProcessRunning(uint16_t);
    const Process    *getRunningProcess(uint16_t);

    //termija
    termija::Pane*   createPane(uint16_t);
    
    
    //keyboard
    KeyboardKey     getPressedKey();
    std::string     getInputTxt();
    void            caseToggle();
    void            setUpperCase();
    void            setLowerCase();
    void            clearKeys();
    void            pressKey(const KeyboardKey); 
    bool            isCtrl();

    //date time
    const Time&            getTime();
    const Date&            getDate();


    //file system
    const std::string&          getWorkingDirectory();
    const std::string&          getRootDirectory();
    const std::string           getWorkingDirectoryRelative();
    bool                        changeWorkingDirectory(const std::string&);
    int8_t                      makeDirectory(const std::string&);
    std::vector<std::string>    listDirectory(const std::string&);
    int8_t                      removeFileOrDir(const std::string&, bool);
    int8_t                      makeFile(const std::string&);
    int8_t                      moveFileOrDir(const std::string&, const std::string&);
    int8_t                      copyFileOrDir(const std::string&, const std::string&, std::filesystem::copy_options);


    friend int ::main(void);
private://accessible only from main
    static Stozer& instance(){
        static Stozer instance;
        return instance;
    }

};

const size_t        SPECIAL_KEYS_SIZE = 32;
const size_t        LAT_KEYS_SIZE = 16;
const KeyboardKey   SPECIAL_KEYS[SPECIAL_KEYS_SIZE] = {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER,
    KEY_PAGE_UP,
    KEY_PAGE_DOWN,
    KEY_SPACE,
    KEY_CAPS_LOCK,
    KEY_BACKSPACE,
    KEY_DELETE,
    KEY_PERIOD,
    KEY_COMMA,
    KEY_BACKSLASH,
    KEY_SLASH,
    KEY_LEFT_BRACKET,
    KEY_RIGHT_BRACKET,
    KEY_MINUS,
    KEY_APOSTROPHE,
    KEY_EQUAL,
    KEY_SEMICOLON,
    KEY_TAB,
    KEY_LEFT_CONTROL,
    KEY_RIGHT_CONTROL
};

const char          SPECIAL_CHARS[SPECIAL_KEYS_SIZE] = {
    ' ',
    '.',
    ',',
    '/',
    ':',
    ';',
    '[',
    ']',
    '\\',
    '-',
    '=',
    '\'',
    '_',
    '<',
    '>',
    ':',
    ';',
    '!',
    '@',
    '#',
    '$',
    '%',
    '^',
    '&',
    '*',
    '(',
    ')',
    '+',
    '"',
    '?'
};

const int          LAT_CHARS[LAT_KEYS_SIZE] = {
    353,//'š',
    352,//'Š',
    273,//'đ',
    272,//'Đ',
    382,//'ž',
    381,//'Ž',
    269,//'č',
    268,//'Č',
    263,//'ć',
    262//'Ć'
};


//starting from GetWorkingDirectory
const std::vector< std::string > REQUIRED_PATHS = {
    "fs",
    "fs\\korisnici",
    "fs\\korisnici\\rs26",
    "fs\\komande"
};



}



#endif