#include "izvrsi.h"
#include <stozer.h>
#include <utils.h>


namespace stozer{



/*
    checks if file exists and has stozer header
*/
bool
Izvrsi::isStozerFile(const std::string &path){
    return filesystem::is_stozer_file(path + filesystem::FILE_EXTENSION);
}

}