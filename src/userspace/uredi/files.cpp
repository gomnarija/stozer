#include "uredi.h"
#include <utils.h>
#include <rope.h>

#include <plog/Log.h>
#include <fstream>



namespace stozer{


/*
    checks if given file exists
*/
bool Uredi::fileExists(const std::string &relativePath){
    std::string path = this->stozer.getWorkingDirectory();
    return (filesystem::move_path_wv(path, relativePath, this->stozer.getRootDirectory())
                && std::filesystem::is_regular_file(filesystem::path_to_u32(path + this->fileExtension)));
}

/*
    loades from text file into textBox
*/
bool Uredi::loadFromFile(const std::string &relativePath){
    std::string path = this->stozer.getWorkingDirectory();
    if(!filesystem::move_path(path, relativePath, this->stozer.getRootDirectory()))
        return false;

    std::ifstream file(std::filesystem::u8path(path + fileExtension));
    if(!file.is_open())
        return false;


    //clear textBox
    this->textBox->clear();
    //file has to have stozer header
    std::string header;
    std::getline(file, header);
    if(header != filesystem::FILE_HEADER){
        file.close();
        return false;
    }
    //read into textBox line by line, jumped over header
    std::string line;
    while(std::getline(file, line)){
        this->textBox->insertLineAtCursor(line.c_str());
    }
    //done
    file.close();
    return !file.is_open();
}

/*
    saves from textBox to text file
*/
bool Uredi::saveToFile(const std::string &relativePath){
    std::string path = this->stozer.getWorkingDirectory();
    if(!filesystem::move_path(path, relativePath, this->stozer.getRootDirectory()))
        return false;

    std::ofstream file(std::filesystem::u8path(path + fileExtension));
    if(!file.is_open())
        return false;


    //add stozer header
    file << filesystem::FILE_HEADER << "\n";
    //read line by line from textBox, saving to file
    RopeLeafIterator lit = this->textBox->getRopeLeafIterator();
    RopeNode *current;
    while((current = lit.pop()) != nullptr){
        if(current->text != nullptr){
            file << current->text.get();
            if(has_flags(current->flags.get(), FLAG_NEW_LINE)){
                file << "\n";
            }
        }
    }
    //done
    file.close();
    return !file.is_open();
}



}
