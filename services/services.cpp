
#include <cstring>
#include "services.h"

std::string get_greeting(){
    std::string greet {"Hello!"};
    return greet;
}

int get_time_str(char *time_str){
    // https://stackoverflow.com/a/49721378/11199445
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    strncpy(time_str, ss.str().c_str(), ss.str().length());
    return EXIT_SUCCESS;
}