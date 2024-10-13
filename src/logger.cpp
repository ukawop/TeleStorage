#include "logger.h"
#include <chrono>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    logFile << "[" << getCurrentTime() << "] " << message << std::endl;
}

void Logger::logError(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    errorFile << "[" << getCurrentTime() << "] " << message << std::endl;
}

std::string Logger::getCurrentTime() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}

Logger::Logger() {
    logFile.open("output.log", std::ios::app);
    errorFile.open("error.log", std::ios::app);
}

Logger::~Logger() {
    if (logFile.is_open()) logFile.close();
    if (errorFile.is_open()) errorFile.close();
}

std::ofstream logFile;
std::ofstream errorFile;
std::mutex logMutex;
