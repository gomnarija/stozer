#include "uredi.h"
#include <utils.h>

#include <map>

#include <plog/Log.h>

namespace stozer{





void Uredi::setup_box(){
    termija::Termija &termija = termija::tra_get_instance();
    uint16_t textWidth = (termija.fontWidth+termija.fontSpacing);
    uint16_t textHeight = (termija.fontHeight+termija.fontSpacing);
    //box
    //position
    uint16_t x = ((float)this->pane->width / 100) * this->boxMargin;//make space for fileName
    uint16_t y = ((float)this->pane->height / 100) * this->boxMargin + (termija.fontHeight+termija.fontSpacing);
    //size
    uint16_t width  = this->pane->width - (x * 2);
    uint16_t height = this->pane->height - (y + ((float)this->pane->height / 100) * this->boxMargin);
    this->box = (termija::Box*)termija::tra_add_widget(*(this->pane),
                                            std::make_unique<termija::Box>(x, y, width, height));
}



void Uredi::setup_options_section(){
    if(this->box == nullptr)
        return;

    termija::Termija &termija = termija::tra_get_instance();
    uint16_t fontWidth = (termija.fontWidth+termija.fontSpacing);
    uint16_t fontHeight = (termija.fontHeight+termija.fontSpacing);
    uint8_t distance = ((float)(this->box->getWidth() - this->optionsSectionWidthMargin*2*fontWidth)/ this->entriesPerRow);//distance from beggining of one entry to next
    distance -= (distance%fontWidth);//this is to that it fits with text. i think

    //starting position, bottomLeft of the box + margins
    uint16_t x = this->box->getX() + (this->optionsSectionWidthMargin * fontWidth);
    uint16_t y = (this->box->getY() + this->box->getHeight() - fontHeight) - (this->optionsSectionHeightMargin * fontHeight);
    size_t done=0;
    for(const auto &enit : this->optionsSectionEntries){
        //create widget
        this->optionsSectionTextVector.push_back((termija::Text*)termija::tra_add_widget(*(this->pane),
                std::make_unique<termija::Text>(x, y, "")));
        termija::Text *text = this->optionsSectionTextVector.at(this->optionsSectionTextVector.size()-1);
        //insert entry
        text->insertAt(enit.first.c_str(), 0,  FLAG_INVERT);
        uint16_t spaceLeft = (distance / fontWidth) - enit.first.length();
        text->insertAt(string::truncate_string(enit.second, spaceLeft).c_str(), text->getTextWidth()-1);
        //move x,y
        done++;
        if(done%this->entriesPerRow!=0 && (x+distance) < (this->box->getWidth() - this->optionsSectionWidthMargin*fontWidth)){
            x += distance;   
        }else{
            this->optionsSectionRows++;
            y -= (fontHeight);
            x =  this->box->getX() + (this->optionsSectionWidthMargin * fontWidth);
        }
    }
}

void Uredi::setup_file_name(){
    if(this->box == nullptr)
        return;

    termija::Termija &termija = termija::tra_get_instance();
    uint16_t fontWidth = (termija.fontWidth+termija.fontSpacing);
    uint16_t fontHeight = (termija.fontHeight+termija.fontSpacing);
    
    //position, left above box
    uint16_t x = ((float)this->pane->width / 100) * this->boxMargin;
    uint16_t y = ((float)this->pane->height / 100) * this->boxMargin;
    this->fileNameText = (termija::Text*)termija::tra_add_widget(*(this->pane),
                std::make_unique<termija::Text>(x, y, ""));
    this->fileNameText->insertAt(string::truncate_string(this->getFileName(), this->maxFileNameLength).c_str(), 0, FLAG_INVERT);
}



void Uredi::setup_text_box(){
    if(this->box == nullptr)
        return;

    termija::Termija &termija = termija::tra_get_instance();
    uint16_t fontWidth = (termija.fontWidth+termija.fontSpacing);
    uint16_t fontHeight = (termija.fontHeight+termija.fontSpacing);
    uint16_t marginWidth = ((float)(this->box->getWidth()/100) * this->textBoxMargin);
    uint16_t marginHeight = ((float)(this->box->getHeight()/100) * this->textBoxMargin);
    //position, inside box + margin
    uint16_t x = this->box->getX() + marginWidth;
    uint16_t y = this->box->getY() + marginHeight;
    //size
    uint16_t width = std::max((this->box->getWidth() - (2*marginWidth)) / fontWidth, 1);
    uint16_t height = std::max(((this->box->getHeight() - (2*marginHeight)) / fontHeight) - (get_options_section_height() / fontHeight), 1);

    this->textBox = (termija::TextBox*)termija::tra_add_widget(*(this->pane),
                std::make_unique<termija::TextBox>(x, y, width, height));
}



uint16_t Uredi::get_options_section_height(){
    termija::Termija &termija = termija::tra_get_instance();
    uint16_t fontHeight = (termija.fontHeight+termija.fontSpacing);
    return (fontHeight * this->optionsSectionRows) + (this->optionsSectionHeightMargin * fontHeight);
}



};