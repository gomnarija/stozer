#ifndef UTILS_H
#define UTILS_H

#include <stozer.h>

namespace stozer{

//
//  STRING
//
namespace string{
std::string                                 trim_string(std::string);
std::vector<std::string>                    split_string(const std::string&, const std::string&); 
std::pair<std::string, std::string>         split_string_into_two(const std::string&, const std::string&); 
}

//
//  DATE-TIME
//
namespace datetime{
uint8_t                                     days_in_a_month(const stozer::Date &);

}



}



#endif