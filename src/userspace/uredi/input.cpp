#include "uredi.h"

#include <plog/Log.h>

namespace stozer{



void Uredi::handle_text(std::string &input){
    //input to textBox
    if(this->textBox != nullptr){
        this->textBox->insertAtCursor(input.c_str());
        this->textBox->findCursor();
    }
}

void Uredi::handle_command(KeyboardKey key){
    if(!this->stozer.isCtrl())
        return;
    //commands
    //TODO: caps lock
    if(this->controlKeysMap.find(key) != this->controlKeysMap.end()){
        (this->*(this->controlKeysMap.at(key)))();//sacuvaj boze
    }
}


void Uredi::handle_key(KeyboardKey input){
    //textBox
    if(input == KEY_DELETE)
        this->textBox->deleteAtCursor();
    if(input == KEY_BACKSPACE && this->textBox->getCurrentIndex() > 0){
        this->textBox->backspaceAtCursor();
        this->textBox->findCursor();
    }

        //moving cursor
    if(input == KEY_LEFT && textBox->getCurrentIndex() > 0){
        this->textBox->cursorWalkLeft(1);
        this->textBox->findCursor();
    }
    if(input == KEY_RIGHT){
        this->textBox->cursorWalkRight(1);
        this->textBox->findCursor();
    }

    //move cursor up
    if(input == KEY_UP && textBox->getCurrentIndex() > 0){
        this->textBox->cursorWalkUp(1);
        this->textBox->findCursor();
    }
    if(input == KEY_DOWN){
        this->textBox->cursorWalkDown(1);
        this->textBox->findCursor();
    }
        //scrolling
    if(input == KEY_PAGE_DOWN){
        this->textBox->frameCursorMove(1);
    }
    if(input == KEY_PAGE_UP){
        this->textBox->frameCursorMove(-1);
    }
        //new line
    if(input == KEY_ENTER){
        this->textBox->insertAtCursor(" ");//this will serve as \n char
        this->textBox->insertFlagAtRange(this->textBox->getCurrentIndex()-1, 1, FLAG_NEW_LINE);
        this->textBox->findCursor();
    }

            //tab - 4 spaces TODO : config
    if(input == KEY_TAB){
        this->textBox->insertAtCursor("    ");
        this->textBox->findCursor();
    }
}



};