#pragma once

#ifndef LOG_H
#define LOG_H

#include <string>
#include <iostream>

enum LogLevel {
    LOG_LEVEL_NONE,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG
};

extern LogLevel currentLogLevel;

void setLogLevel(LogLevel level);
void logMessage(LogLevel level, const std::string &message);

#endif // LOG_H