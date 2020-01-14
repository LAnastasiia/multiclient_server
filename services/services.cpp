#include <string>
#include "services.h"

std::string construct_greeting(){
    std::string greet {"Hello!"};
    return greet;
}


char* get_time_str() {
    time_t now = time(0);
    return reinterpret_cast<char *>(localtime(&now)->tm_hour);
}