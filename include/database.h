#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>
#include <cstdint>

class Database {
public:
    Database(const std::string& dbPath);

    ~Database();

    std::uint64_t addFile(const std::string& idList, const std::string& fileName, size_t fileSize, const std::string& time);

    void deleteFile(std::uint64_t uniqueId);

    std::string getIdList(std::uint64_t uniqueId);

    std::vector<std::string> getLastFile();

    std::vector<std::vector<std::string>> getAllFiles();

private:
    sqlite3* db;

    void createTable();
};

#endif
