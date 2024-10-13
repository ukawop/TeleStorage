#include "env.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

Env::Env(const std::string& path) : path(path) {}

void Env::loadEnv() {
    std::ifstream envFile(path);
    if (!envFile.is_open()) {
        std::string errmsg = "Ошибка открытия файла: " + path;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }

    std::string line;
    while (std::getline(envFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        key.erase(std::remove_if(key.begin(), key.end(), isspace), key.end());
        value.erase(std::remove_if(value.begin(), value.end(), isspace), value.end());

        setenv(key.c_str(), value.c_str(), 1);
    }

    envFile.close();
}

void Env::updateEnvFile(const std::string &key, const std::string &value) {
    std::ifstream envFile(path);
    if (!envFile.is_open()) {
        std::string errmsg = "Ошибка открытия файла: " + path;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }

    std::vector<std::string> lines;
    bool keyFound = false;

    std::string line;
    while (std::getline(envFile, line)) {
        if (line.find(key + "=") == 0) {
            lines.push_back(key + "=" + value);
            keyFound = true;
        } else {
            lines.push_back(line);
        }
    }

    if (!keyFound) {
        lines.push_back(key + "=" + value);
    }

    envFile.close();

    std::ofstream outFile(path);
    if (!outFile.is_open()) {
        std::string errmsg = "Ошибка открытия файла: " + path;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }

    for (const auto &l : lines) {
        outFile << l << std::endl;
    }
    outFile.close();
}

void Env::setEnvVar(const std::string &key, const std::string &value) {
    if (setenv(key.c_str(), value.c_str(), 1) == 0) {
        updateEnvFile(key, value);
    } else {
        std::string errmsg = "The environment value could not be set: " + key;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }
}

const std::string Env::getEnvVar(const std::string &key) {
    const char* value = std::getenv(key.c_str());
    return value ? std::string(value) : std::string();
}
