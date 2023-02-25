/*
    Krckov  reformirani shell 
*/


#include <stozer.h>


namespace stozer{

Krsh::Krsh(Stozer &stozer) : Process(stozer){

    this->name              = "krsh";//unique
    
}


void Krsh::setup(){

}

void Krsh::update(){
    this->stozer.sout("muda");
}

void Krsh::cleanup(){
    
}

}