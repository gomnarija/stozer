#ifndef STOZER_H
#define STOZER_H


#include <termija.h>

#include <vector>
#include <stack>
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

    std::unordered_map
        < std::string, std::unique_ptr<Process> >               loaded;
    
    std::unordered_map
        < uint16_t, std::unique_ptr<Process> >               running;

    //rope buffers
    std::unique_ptr<RopeNode>                                   ropeout;
    std::unique_ptr<RopeNode>                                   ropein;

    //currently active/drawn Processes
    std::stack<uint16_t>                                        frontStack;

    //cycle
    void        start();
    void        update();
    void        end();
    //util
    uint16_t       get_next_PID();
    bool           loaded_exists_with_name(const std::string);
    bool           running_exists_with_PID(uint16_t);


public:
    bool                                                        shouldEnd;
    termija::Pane                                              *pane;


    uint16_t                                                     getFrontPID();
    void                                                         setFrontPID(uint16_t);

    //operations
    int8_t           processLoad(std::unique_ptr<Process>);
    uint16_t         processRun(const std::string);
    int8_t           processTerminate(uint16_t);
    
    
    void            sout(const std::string);//standard out



    friend int ::main(void);
private://accessible only from main
    static Stozer& instance(){
        static Stozer instance;
        return instance;
    }

};



}



#endif