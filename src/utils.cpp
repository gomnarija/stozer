#include "utils.h"

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
    result.push_back(str.substr(last));
    return result;
} 


/*
    splits string into two at delimeter, 
        if there is no delimiter in string both sides will be equal to given string
*/
std::pair<std::string, std::string> split_string_into_two(const std::string& str, const std::string &delimiter){
    std::string left, right;
    size_t split = str.find(delimiter);
    if(split != std::string::npos && split+1 < str.length()){
        left = str.substr(0, split);
        right = str.substr(split+1);
    }else{
        left = right = str;
    }

    return std::pair<std::string, std::string>(left, right);
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



}