#include "utils.h"
#include <raylib.h>
#include <plog/Log.h>
#include <filesystem>
#include <algorithm>
#include <rope.h>
#include <fstream>

namespace stozer{

namespace string{

/*
    removes leading and trailing whitespace characters from the given string
        returns new string, doesn't modify the given one
*/
std::string trim_string(std::string str){
    const auto strBegin = str.find_first_not_of(" ");
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(" ");
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


/*
    leaves only one whitespace char in a row
*/
std::string remove_extra_whitespace(std::string str){
    size_t i=0;
    while ((i=str.find_first_of(' ', i)) != std::string::npos)
    {
        size_t j = std::min(str.find_first_not_of(" ", i), str.length());
        str.replace(i, j-i, " ");
        i++;
    }
    
    return str;
}


std::string replace_all(std::string &str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;

    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

/*
    tokenizes given string using given delimeter
*/
std::vector<std::string> split_string(const std::string& str, const std::string &delimiter){
    std::vector<std::string> result;
    size_t last = 0; 
    size_t next = 0; 
    while ((next = str.find(delimiter, last)) != std::string::npos) {   
        result.push_back(str.substr(last, next-last));   
        last = next + 1; 
    }
    if(last <= str.length()-1)
        result.push_back(str.substr(last));
    return result;
} 


/*
    splits string into two at delimeter, 
        if there is no delimiter in string first = str, second = <empty>
*/
std::pair<std::string, std::string> split_string_into_two(const std::string& str, const std::string &delimiter){
    std::string left, right;
    size_t split = str.find(delimiter);
    if(split != std::string::npos && split+1 < str.length()){
        left = str.substr(0, split);
        right = str.substr(split+1);
    }else{
        left = str;
        right = "";
    }

    return std::pair<std::string, std::string>(left, right);
}


/*
    truncates given string if it goes over given length,
        replaceing last 3 chars with ...
*/
std::string truncate_string(std::string str, size_t length){
    if(str.length() <= length)
        return str;

    str = str.substr(0, length);
    for(int i=str.length()-1;i>0&&i>str.length()-4;i--)
        str.at(i) = '.';

    return str;
}

}





namespace datetime{

/*
    returns how many days are in a given dates current month
*/
uint8_t days_in_a_month(const stozer::Date &date){
    if((date.months==2) and ((date.years%4==0)  or ((date.years%100==0) and (date.years%400==0))))
        return 29;
    else if(date.months==2)
        return 28;
    else if(date.months == 1 || date.months == 3 || date.months == 5 || 
                date.months == 7 || date.months == 8 || date.months == 10 || date.months == 12)
        return 31;
    else
        return 30;

}


}




namespace filesystem{


std::string _separator_replace(std::string &path){
    return string::replace_all(path, "/", "\\");
}

/*
    checks if directory or file exists at path
*/
bool is_valid_path(const std::string &path){
    //raylib threats this as valid, it shouldn't be
    if(path.find("...") != std::string::npos){
        return false;
    }

    if(std::filesystem::is_directory(path_to_u32(path))){
        return true;
    }
    
    if( path.length() > 4 && path.substr(path.length()-4) != ".txt" ){
        return std::filesystem::is_regular_file(path_to_u32(path + ".txt"));
    }else{
        return std::filesystem::is_regular_file(path_to_u32(path));
    }
    
    
}

bool _is_lat(int c){
    for(int i=0;i<LAT_KEYS_SIZE;i++){
        if(c == LAT_CHARS[i])
            return true;
    }
    return false;
}

bool _is_not_alphanum_or_underscore(int c){
    return !(isalnum(c) || c == '_' || _is_lat(c));
}

/*
    alphanumeric and '_'
*/
bool is_valid_name(const std::string &name){
    if(name.empty())
        return false;

    const char *s;
    s =name.c_str();
    int p = 0, i = 0;
    size_t size = ustrlen(s); 
    while(i < size){
        int c = GetCodepoint(s, &p);
        if(_is_not_alphanum_or_underscore(c))
            return false;
        //invalid
        if (c == 0x3f)
            return false;

        i+=p;
    }
    return true;
}


std::string get_name(const std::string &name){
    if(name.empty())
        return "";

    std::string nname = std::string(name);
    _separator_replace(nname);

    size_t start    = nname.find_last_of(SEPARATOR)+1;
    start           = start == std::string::npos ? 0 : start;
    size_t end      = nname.at(nname.size()-1) == SEPARATOR ? nname.size()-1 : nname.size();
    return nname.substr(start, end);
}


/*
    tries to move given path
*/
bool move_path(std::string &path,const std::string &relativePath, const std::string &rootPath){
    if(relativePath.empty())
        return true;
    

    std::string pPath = std::string(path);
    std::string rPath = std::string(relativePath);
    _separator_replace(rPath);


    //from root
    if(rPath.at(0) == '\\'){
        if(rPath.length() > 1){
            pPath = rootPath;
            rPath = rPath.substr(1);
        }else{
            path = rootPath;
            return true;
        }
    }



    rPath += rPath.at(rPath.length()-1) != '\\' ? "\\" : "";//for easier tokenazing
    size_t i = 0, j = rPath.find("\\");
    while(i<j && j != std::string::npos){
        std::string pathToken = rPath.substr(i, j-i);
        if(pathToken == "."){
            //stay here
        }else if(pathToken == ".."){
            //try to go back
            size_t sep = pPath.find_last_of("\\");
            if(sep != std::string::npos){
                pPath = pPath.substr(0, sep);
            }else{
                return false;
            }
        }else if(pathToken == "..."){
            //this is invalid, but Windows used to thread it as "go to grandparent directory"
            return false;
        }
        else{
            //try to go forward
            if(is_valid_path(pPath + SEPARATOR + pathToken)){
                pPath += SEPARATOR + pathToken;
            }else{
                return false;
            }

        }
        
        i=j+1;
        j = rPath.find("\\", j+1);
    }

    if(i==j)//multiple \\ in a row
        return false;
    
    //success
    path = pPath;
    return true;
}

/*
    moves path, without validation
*/
bool move_path_wv(std::string &path,const std::string &relativePath, const std::string &rootPath){
    if(relativePath.empty())
        return true;
    

    std::string pPath = std::string(path);
    std::string rPath = std::string(relativePath);
    _separator_replace(rPath);


    //from root
    if(rPath.at(0) == '\\'){
        if(rPath.length() > 1){
            pPath = rootPath;
            rPath = rPath.substr(1);
        }else{
            path = rootPath;
            return true;
        }
    }

    rPath += rPath.at(rPath.length()-1) != '\\' ? "\\" : "";//for easier tokenazing
    size_t i = 0, j = rPath.find("\\");
    while(i<j && j != std::string::npos){
        std::string pathToken = rPath.substr(i, j-i);
        if(pathToken == "."){
            //stay here
        }else if(pathToken == ".."){
            //try to go back
            size_t sep = pPath.find_last_of("\\");
            if(sep != std::string::npos){
                pPath = pPath.substr(0, sep);
            }else{
                return false;
            }
        }else{
            //try to go forward
            if(true){
                pPath += SEPARATOR + pathToken;
            }else{
                return false;
            }

        }
        
        i=j+1;
        j = rPath.find("\\", j+1);
    }

    if(i==j)//multiple \\ in a row
        return false;
    
    //success
    path = pPath;
    return true;
}


bool is_inside(const std::string &parent, const std::string &child){
    return parent.length() <= child.length() &&
        child.substr(0, parent.length()) == parent;
}

/*
    returns relative path, starting from given root
*/
std::string relative_path(const std::string &rootPath, const std::string &path){
    if(rootPath.length() < path.length() &&
        path.substr(0, rootPath.length()) == rootPath){
            return path.substr(rootPath.length());
    }else if(rootPath == path){
        return "\\";
    }else{
        return path;
    }
}

/*
    returns real path, starting from given root
*/
std::string real_path(const std::string &rootPath, const std::string &path){
    return rootPath + path;
}


/*
    checks if given file exist and has .txt extension
*/
bool is_txt(const std::string &path){
    return is_valid_path(path) &&
                path.length() > 4 &&
                    path.substr(path.length()-4) == ".txt";

}

/*
    checks if there is a dir at path
*/
bool is_dir(const std::string &path){
    return is_valid_path(path) && 
        std::filesystem::is_directory(path_to_u32(path));

}

/*
    checks if the dir is empty
*/
bool is_dir_empty(const std::string &path){
    return is_valid_path(path) && 
            std::filesystem::is_empty(path_to_u32(path));

}

std::u32string path_to_u32(const std::string &path){
    std::filesystem::path pu = std::filesystem::u8path(path);
    return pu.u32string();
}



/*
    checks if file exists and has stozer header
*/
bool is_stozer_file(const std::string &path){
    std::ifstream file(std::filesystem::u8path(path));
    if(!file.is_open())
        return false;

    //file has to have stozer header
    std::string header;
    std::getline(file, header);
    if(header != filesystem::FILE_HEADER){
        file.close();
        return false;
    }

    file.close();
    return !file.is_open();
}


}


}