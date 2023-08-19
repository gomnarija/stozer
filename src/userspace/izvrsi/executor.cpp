#include "izvrsi.h"
#include <stozer.h>
#include <momo.h>
#include <reader.h>
#include <utils.h>

#include <fstream>

namespace stozer{



void
Izvrsi::executeSourceFile(const std::string &path){
    //open file
    std::ifstream file(std::filesystem::u8path(path + filesystem::FILE_EXTENSION));
    if(!file.is_open()){
        *(this->outStream)  << "greška pri otvaranju fajla. unesi argument -p za pomoć.";
        return;
    }

    //go over header
    std::string header;
    std::getline(file, header);

    //set file name
    momo::LOG_FILENAME = momo::get_file_name(path);


    //read file
    momo::moListPtr result = momo::read_file(file);
    this->printLog();

    //eval and print last one
    momo::moValPtr resultVal = momo::NIL;
    for(int i=0;i<result->size();i++){
        resultVal = momo::eval(result->at(i), this->globalEnv);
        this->printLog();
    }


    *(this->outStream)  << resultVal->print() << '\n';
}




}