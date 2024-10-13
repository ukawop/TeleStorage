#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <iostream>
#include <mutex>
#include <fstream>

class Logger {
public:
    static Logger& getInstance();

    void log(const std::string& message);

    void logError(const std::string& message);

    std::string getCurrentTime();

private:
    std::ofstream logFile;
    std::ofstream errorFile;
    std::mutex logMutex;

    Logger();

    ~Logger();
};

#define LOG(message) Logger::getInstance().log(message)
#define LOG_ERROR(message) Logger::getInstance().logError(message)
#endif
