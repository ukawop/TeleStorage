#include "database.h"
#include "logger.h"

Database::Database(const std::string& dbPath) {
        if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
            std::string errmsg = "Error opening the database: " + std::string(sqlite3_errmsg(db));
            LOG_ERROR(errmsg);
            std::cerr << errmsg << std::endl;
        } else {
            createTable();
        }
}

Database::~Database() {
    sqlite3_close(db);
}

void Database::createTable() {
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "CREATE TABLE IF NOT EXISTS files ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "id_list TEXT NOT NULL,"
                      "file_name TEXT NOT NULL,"
                      "file_size INTEGER NOT NULL,"
                      "time TEXT NOT NULL);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::string errmsg = "Error creating the table: " + std::string(sqlite3_errmsg(db));
            LOG_ERROR(errmsg);
            std::cerr << errmsg << std::endl;
        }
    } else {
        std::string errmsg = "Error preparing the statement: " + std::string(sqlite3_errmsg(db));
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
    }

    sqlite3_finalize(stmt);
}

std::uint64_t Database::addFile(const std::string& idList, const std::string& fileName, size_t fileSize, const std::string& time) {
    std::string sql = "INSERT INTO files (id_list, file_name, file_size, time) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, idList.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, fileName.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, fileSize);
        sqlite3_bind_text(stmt, 4, time.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::string errmsg = "Error adding the file: " + std::string(sqlite3_errmsg(db));
            LOG_ERROR(errmsg);
            std::cerr << errmsg << std::endl;
            sqlite3_finalize(stmt);
            return -1;
        }
        std::uint64_t id = sqlite3_last_insert_rowid(db);
        sqlite3_finalize(stmt);
        return id;
    }
    std::string errmsg = "Error before adding the file: " + std::string(sqlite3_errmsg(db));
    LOG_ERROR(errmsg);
    std::cerr << errmsg << std::endl;
    sqlite3_finalize(stmt);
    return -1;
}

void Database::deleteFile(std::uint64_t uniqueId) {
    std::string sql = "DELETE FROM files WHERE id = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, uniqueId);

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            std::string errmsg = "Error deleting file: " + std::string(sqlite3_errmsg(db));
            LOG_ERROR(errmsg);
            std::cerr << errmsg << std::endl;
        }
    } else {
        std::string errmsg = "Error before deleting file: " + std::string(sqlite3_errmsg(db));
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
    }
    sqlite3_finalize(stmt);
}

std::string Database::getIdList(std::uint64_t uniqueId) {
    std::string sql = "SELECT id_list FROM files WHERE id = ?;";
    sqlite3_stmt* stmt;
    std::string idList;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, uniqueId);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            idList = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        } else {
            std::string errmsg = "The file with ID " + std::string(sqlite3_errmsg(db)) + " was not found";
            LOG_ERROR(errmsg);
            std::cerr << errmsg << std::endl;
        }
    } else {
        std::string errmsg = "Error preparing the request: " + std::string(sqlite3_errmsg(db));
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
    }

    sqlite3_finalize(stmt);
    return idList;
}

std::vector<std::vector<std::string>> Database::getAllFiles() {
    std::vector<std::vector<std::string>> files;
    std::string sql = "SELECT id, id_list, file_name, file_size, time FROM files;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> fileData;
            fileData.push_back(std::to_string(sqlite3_column_int(stmt, 0))); // id
            fileData.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))); // id_list
            fileData.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))); // file_name
            fileData.push_back(std::to_string(sqlite3_column_int(stmt, 3))); // file_size
            fileData.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))); // time
            files.push_back(fileData);
        }
    } else {
        std::string errmsg = "Error preparing the request: " + std::string(sqlite3_errmsg(db));
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
    }
    sqlite3_finalize(stmt);
    return files;
}

std::vector<std::string> Database::getLastFile() {
    std::vector<std::string> lastFile;
    std::string sql = "SELECT id, id_list, file_name, file_size, time FROM files ORDER BY id DESC LIMIT 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            lastFile.push_back(std::to_string(sqlite3_column_int(stmt, 0))); // id
            lastFile.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))); // id_list
            lastFile.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))); // file_name
            lastFile.push_back(std::to_string(sqlite3_column_int(stmt, 3))); // file_size
            lastFile.push_back(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))); // time
        } else {
            std::string errmsg = "Error get last file: " + std::string(sqlite3_errmsg(db));
            LOG_ERROR(errmsg);
            std::cerr << errmsg << std::endl;

        }
    } else {
        std::string errmsg = "Error preparing the request: " + std::string(sqlite3_errmsg(db));
        LOG_ERROR(errmsg);
        std::cerr << errmsg << std::endl;
    }
    sqlite3_finalize(stmt);
    return lastFile;
}
