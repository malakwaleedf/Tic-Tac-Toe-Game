#pragma once

const int TIME_BUFFER_SIZE = 26;

std::string timeToString(std::chrono::system_clock::time_point timePoint);