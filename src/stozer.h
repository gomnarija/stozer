#ifndef STOZER_H
#define STOZER_H


#include <termija.h>
#include <raylib.h>

#include <vector>
#include <stack>
#include <queue>
#include <unordered_map>
#include <string>
#include <memory>



//forward declaration of the main function in global namespace
int main(void);

namespace stozer{


class Stozer;


//process
class Process{
protected:
    Process(Stozer &stozer);

    std::string                     name;//unique
    uint16_t                        PID=0;//0 default,starts from 1

public:
    Stozer                         &stozer;
    
    const std::string &             getName();
    uint16_t                        getPID();
    void                            setPID(uint16_t);

    //cycle
    virtual void setup()=0;
    virtual void update()=0;
    virtual void cleanup()=0;
    virtual void draw()=0;


    virtual Process*                instantiate()=0;
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

    //rope buffers
    std::unique_ptr<RopeNode>                                   ropeout;
    std::unique_ptr<RopeNode>                                   ropein;

    //currently active/drawn Processes
    std::stack<uint16_t>                                        frontStack;
    std::unordered_map
        < uint16_t, termija::Pane* >                            frontPaneMap;

    //key buffer
    std::queue<KeyboardKey>                                     keyQueue;

    //text buffers
    std::string                                                 stin;
    std::string                                                 stout;

    //cycle
    void        start();
    void        update();
    void        end();
    //util
    uint16_t       get_next_PID();



public:
    bool                                                         shouldEnd;


    uint16_t                                                     getFrontPID();
    void                                                         setFrontPID(uint16_t);

    //process operations
    int8_t           processLoad(std::unique_ptr<Process>);
    uint16_t         processRun(const std::string);
    int8_t           processTerminate(uint16_t);

    //termija
    termija::Pane*   createPane(uint16_t);
    
    
    //keyboard
    KeyboardKey     getPressedKey();
    void            clearKeys();
    void            pressKey(const KeyboardKey); 


    //text flow
        //input
    void            stdInPut(const std::string);
    std::string     stdInPull();
    void            stdInClear();
        //output
    void            stdOutPut(const std::string);
    std::string     stdOutPull();
    void            stdOutClear();

    friend int ::main(void);
private://accessible only from main
    static Stozer& instance(){
        static Stozer instance;
        return instance;
    }

};

const size_t        SPECIAL_KEYS_SIZE = 32;
const KeyboardKey   SPECIAL_KEYS[SPECIAL_KEYS_SIZE] = {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_ENTER
};


}



#endif