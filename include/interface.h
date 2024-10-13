#ifndef INTERFACE_H
#define INTERFACE_H
#include "database.h"
#include "file_manger.h"
#include <vector>
#include <string>
#include <ncurses.h>

class Interface {
public:
    Interface(FileManager& fm);

    void run();

private:
    Database& db;
    FileManager& fm;
    std::vector<std::vector<std::string>> files;
    int selectedIndex;

    void displayFiles();

    void handleInput(int ch);

    void promptForFilePath();

    void displayHints();
};


#endif
