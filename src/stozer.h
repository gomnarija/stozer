#include <termija.h>

#include <vector>
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
};

//singleton
class Stozer{
    Stozer();
    Stozer(const Stozer&)           = delete;

private:
    std::vector<std::unique_ptr<Process>>                       loaded;
    std::vector<std::unique_ptr<Process>>                       running;
    //currently active/drawn Process
    uint8_t                                                     activePID;
    Process                                                    *active;
    //rope buffers
    std::unique_ptr<RopeNode>                                   ropeout;
    std::unique_ptr<RopeNode>                                   ropein;
    //cycle
    void        start();
    void        update();
    void        end();
    //util
    size_t         find_loaded_by_name(const std::string);
    size_t         find_running_by_PID(const uint8_t);
    uint16_t       get_next_PID(); 


public:
    bool                                                        shouldEnd;
    termija::Pane                                              *pane;

    //operations
    int8_t         processLoad(std::unique_ptr<Process>);
    int8_t         processRun(const std::string);
    
    
    void            sout(const std::string);//standard out



    friend int ::main(void);
private://accessible only from main
    static Stozer& instance(){
        static Stozer instance;
        return instance;
    }

};



// maybe move from here ? TODO:
class Krsh : public Process{
public:
    //cycle
    void setup() override;
    void update() override;
    void cleanup() override;


public:
    Krsh(stozer::Stozer &);



};





}