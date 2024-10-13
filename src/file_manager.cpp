#include "file_manger.h"
#include "paths.h"
#include "logger.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <pwd.h>
#include <unistd.h>

FileManager::FileManager(Api& api, Database& db) : api(api), db(db) {}

int64_t FileManager::sendFileParts(const std::string& filePath) {
    std::vector<std::string> chunks;
    
    splitFile(expandTilde(filePath), chunks);

    std::string id_list;
    size_t fileSize = chunkSize * (chunks.size() - 1);
    std::string date;
    TgBot::Message::Ptr response;

    int step = 99 / chunks.size();
    int progress = 0;
    printProgress(progress, step);
    for (const auto& chunk : chunks) {
        response = api.sendFile(chunk);

        if (!id_list.empty()) {
            id_list += ',';
        }
        id_list += response->document->fileId;
        std::remove(chunk.c_str());
        printProgress(progress, step);
    }
    fileSize += response->document->fileSize;
    date = convertToDateTime(response->date);

    auto fileId = db.addFile(id_list, filePath.substr(filePath.find_last_of('/') + 1), fileSize, date);
    if (fileId == -1) {
        std::string errmsg = "Unable to add file to the database";
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }
    LOG("the file has been added to the cloud: " + filePath);
    return fileId;
}

void FileManager::getFileParts(std::uint64_t fileId, const std::string& outputFilePath) {
    std::vector<std::string> chunks;
    std::string id_list_str = db.getIdList(fileId);
    if (id_list_str.empty()) {
        std::string errmsg = "Id list is empty";
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }
    std::vector<std::string> id_list = split(id_list_str, ',');

    int progress = 0;
    int step = 99 / id_list.size();
    printProgress(progress, step);
    for (const auto& id : id_list) {
        chunks.push_back(api.getFile(id, PARTS_PATH));
        printProgress(progress, step);
    }

    collectFile(chunks, expandTilde(outputFilePath));
    LOG("The file was received from the cloud: " + outputFilePath);
}

void FileManager::splitFile(const std::string& filePath, std::vector<std::string>& chunks) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        std::string errmsg = "Unable to open file: " + filePath;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }

    char* buffer = new char[chunkSize];
    size_t chunkIndex = 0;
    std::string fileName = PARTS_PATH + filePath.substr(filePath.find_last_of('/') + 1);

    while (file) {
        file.read(buffer, chunkSize);
        size_t bytesRead = file.gcount();
        if (bytesRead > 0) {
            std::string chunkFileName = fileName + ".part" + std::to_string(chunkIndex++);
            std::ofstream chunkFile(chunkFileName, std::ios::binary);
            chunkFile.write(buffer, bytesRead);
            chunkFile.close();
            chunks.push_back(chunkFileName);
        }
    }

    delete[] buffer;
    file.close();
}

void FileManager::collectFile(const std::vector<std::string>& chunks, const std::string& outputFilePath) {
    std::ofstream outputFile(outputFilePath, std::ios::binary);
    if (!outputFile.is_open()) { 
        std::string errmsg = "Unable to open file: " + outputFilePath;
        LOG_ERROR(errmsg);
        throw std::runtime_error(errmsg);
    }

    for (const auto& chunk : chunks) {
        std::ifstream chunkFile(chunk, std::ios::binary);
        outputFile << chunkFile.rdbuf();
        chunkFile.close();
        std::remove(chunk.c_str());
    }
    outputFile.close();
}

Database& FileManager::getDatabase(){
    return db;
}

std::string FileManager::convertToDateTime(uint32_t timestamp) {
    time_t raw_time = static_cast<time_t>(timestamp);
    struct tm* timeinfo = std::localtime(&raw_time);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

std::vector<std::string> FileManager::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string FileManager::expandTilde(const std::string& path) {
    if (path.empty() || path[0] != '~') {
        return path;
    }

    const char* home = std::getenv("HOME");
    return std::string(home) + path.substr(1);
}

void FileManager::printProgress(int& progress, int step) {
    std::cout << "\rProgress: " << progress << "%";
    std::cout.flush();
    progress += step;
}
