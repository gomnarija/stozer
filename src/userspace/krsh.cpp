/*
    Krckov  reformirani shell 
*/

#include "krsh.h"
#include <stozer.h>
#include <termija.h>

#include <plog/Log.h>

namespace stozer{

Krsh::Krsh(Stozer &stozer) : Process(stozer){

    this->name              = "krsh";//unique
    
}

Process* Krsh::instantiate(){
    return new Krsh(this->stozer);
}




void Krsh::setup(){
    //drawn process; set to front
    this->stozer.setFrontPID(this->PID);

    //impose as current pane
    this->pane = termija::tra_impose_duplicate_pane(this->stozer.pane);

    //create widgets
    this->textBox = (termija::TextBox*)termija::tra_add_widget(*(this->pane),
                                                std::make_unique<termija::TextBox>(50, 50, 10, 10));



    this->textBox->insertAtCursor("muda.muda.muda.muda.muda.muda.muda.muda.kraj :) ");
    this->textBox->insertAtCursor(std::to_string(this->PID).c_str());
    this->textBox->insertAtCursor(std::string(1, char(this->stozer.getPressedKey())).c_str());
}

void Krsh::update(){
    KeyboardKey key = this->stozer.getPressedKey();
    if(std::isalpha(key)){
        this->textBox->insertAtCursor(std::string(1, char(key)).c_str());
    }
}



void Krsh::draw(){
    if(termija::tra_get_current_pane() != this->pane)
        termija::tra_set_current_pane(this->pane);


    this->textBox->update();
}




void Krsh::cleanup(){
    
    //destroy pane
    termija::tra_remove_pane(this->pane);
    
}

}