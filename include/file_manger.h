#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "api.h"
#include "database.h"
#include <string>
#include <vector>

class FileManager {

public:
    FileManager(Api& api, Database& db);
    int64_t sendFileParts(const std::string& filePath);
    void getFileParts(std::uint64_t fileId, const std::string& outputFilePath);
    std::vector<std::string> split(const std::string& str, char delimiter);
    Database& getDatabase();

private:
    Api& api;
    Database& db;
    const size_t chunkSize = 19 * 1024 * 1024;

    void splitFile(const std::string& filePath, std::vector<std::string>& chunks);
    void collectFile(const std::vector<std::string>& chunks, const std::string& outputFilePath);
    void printProgress(int& progress, int step);
    std::string convertToDateTime(uint32_t timestamp);
    std::string expandTilde(const std::string& path);
};
#endif
