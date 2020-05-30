#pragma once
#include <stdio.h>
#include <iostream>
#include <mutex>
#include <sstream>
#include <errno.h>
#include <sys/utsname.h>

#ifndef SOCKET_PATH
    #define SOCKET_PATH "127.0.0.1:8000"
#endif

#ifndef MESSEGE_LENGTH
    #define MESSEGE_LENGTH 10
#endif

struct sending_data{
    char msg[MESSEGE_LENGTH];
    utsname required_info;
    long info;
};


bool check(int val, const char* message){
    if (val == -1) {
        // ignore error of non-blocking "resource temporary unavailable"
        if(errno != EAGAIN)
            perror(message);
        return true;
    }
    else {
        return false;
    }
}

class my_cout : public std::ostringstream
{
public: 
    my_cout() = default;
    ~my_cout()
    {
        std::lock_guard<std::mutex> guard(_mutexPrint);
        std::cout << this->str();
    }
private:
    static std::mutex _mutexPrint;
};

std::mutex my_cout::_mutexPrint{};


