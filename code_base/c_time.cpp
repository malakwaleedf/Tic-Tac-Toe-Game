#include <vector>
#include <limits>
#include <string>
#include <iostream>
#include "sqlite3.h"
#include <cstdint>
#include <conio.h>
#include <chrono>
#include <ctime>
#include <thread>
#include <map>
#include <cstdint>
#include "c_time.h"

using namespace std;



std::string timeToString(std::chrono::system_clock::time_point timePoint) {
    std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
    char buffer[TIME_BUFFER_SIZE];
    errno_t err = ctime_s(buffer, TIME_BUFFER_SIZE, &time);

    if (err != 0) {
        return "Error converting time.";
    }

    return std::string(buffer);
}
