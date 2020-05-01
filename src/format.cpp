#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long up_time) {
    long seconds = up_time%seconds_minutes;
    up_time -= seconds;
    long minutes = (up_time/seconds_minutes)%60;
    up_time -= (seconds_minutes*minutes);
    long hours = up_time/seconds_hours;
    std::ostringstream elapsedTime;
    elapsedTime << std::setw(2) << std::setfill('0') << hours
              << ":" << std::setw(2) << std::setfill('0') << minutes
              << ":" << std::setw(2) << std::setfill('0') << seconds;
    return elapsedTime.str();
    
}

string Format::kB2MB(long memory){
    memory = memory/1024;
    return std::to_string(memory);
}