#ifndef UTILS_H
#define UTILS_H

#include <stozer.h>

namespace stozer{

//
//  STRING
//
namespace string{
std::string                                 trim_string(std::string);
std::string                                 remove_extra_whitespace(std::string);
std::string                                 replace_all(std::string&, const std::string&, const std::string&);
std::vector<std::string>                    split_string(const std::string&, const std::string&); 
std::pair<std::string, std::string>         split_string_into_two(const std::string&, const std::string&);
std::string                                 truncate_string(std::string, size_t);
};

//
//  DATE-TIME
//
namespace datetime{
uint8_t                                     days_in_a_month(const stozer::Date &);

};


//
//  FILE SYSTEM
//

namespace filesystem{
static const char                           SEPARATOR = '\\';
static const std::string                    FILE_HEADER = "[stozer_file]";

bool                                        is_valid_path(const std::string&);
bool                                        is_valid_name(const std::string&);
std::string                                 get_name(const std::string&);
bool                                        move_path(std::string&, const std::string&, const std::string&);
bool                                        move_path_wv(std::string&, const std::string&, const std::string&);
bool                                        is_inside(const std::string&, const std::string&);
std::string                                 relative_path(const std::string &, const std::string &);
std::string                                 real_path(const std::string &, const std::string &);
bool                                        is_txt(const std::string &);
bool                                        is_dir(const std::string &);
bool                                        is_dir_empty(const std::string &);
std::u32string                              path_to_u32(const std::string &);

};

};



#endif