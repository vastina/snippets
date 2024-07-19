#ifndef _LOGER_H_
#define _LOGER_H_

#include <fstream>
#include <iostream>
#include <cstring>
#include <chrono>

namespace vastina_log{

void logtest(const char* buffer);
void logtime(std::fstream &fp);

};    

inline void vastina_log::logtest(const char* buffer){
    std::fstream fp;
    fp.open("log.txt", std::fstream::app);
    fp.write(buffer, strlen(buffer));
    logtime(fp);
    fp.close();
}

inline void vastina_log::logtime(std::fstream &fp){
    auto currentTime = std::chrono::system_clock::now();
    std::time_t currentTime_t = std::chrono::system_clock::to_time_t(currentTime);
    std::tm localTime = *std::localtime(&currentTime_t);
    fp  <<"  "<< "Current local time: "
        << localTime.tm_year + 1900 << "-"  
        << localTime.tm_mon + 1 << "-"       
        << localTime.tm_mday << " "         
        << localTime.tm_hour << ":" 
        << localTime.tm_min << ":" 
        << localTime.tm_sec << "\n\n"  ; 
    
}

#endif