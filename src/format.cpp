#include <string>
#include "format.h"
#include <sstream>
#include <iomanip>

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function

const int HOUR = 60 * 60;
const int MINUTE = 60;

string Format::ElapsedTime(long seconds) {
    int hour = seconds / HOUR % 24;
    int minute = seconds / MINUTE % 60;
    int second = seconds % 60;

    std::stringstream ss;
    try{
    ss << std::setfill('0') << std::setw(2) << hour << ":"
        << std::setfill('0') << std::setw(2) << minute << ":"
        << std::setfill('0') << std::setw(2) << second;
    return ss.str();
    } catch (...) {
        return "00:00:00";
    }
    }