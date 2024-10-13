#include "interface.h"
#include "logger.h"
#include "paths.h"

Interface::Interface(FileManager& fm)
    : fm(fm), db(fm.getDatabase()), selectedIndex(0) {
    files = db.getAllFiles();
}

void Interface::run() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (true) {
        displayFiles();
        int ch = getch();
        handleInput(ch);
    }

    endwin();
}

void Interface::displayFiles() {
    clear();

    const int columnWidth1 = 7;
    const int columnWidth2 = 40;
    const int columnWidth3 = 20;
    const int columnWidth4 = 20;
     printw("%-*s %-*s %-*s %-*s\n",
               columnWidth1, "ID",
               columnWidth2, "NAME",
               columnWidth3, "SIZE",
               columnWidth4, "DATE");

    auto truncate = [](const std::string& str, size_t maxLength) {
        return str.length() > maxLength ? str.substr(0, maxLength) : str;
    };

    for (size_t i = 0; i < files.size(); ++i) {
        if (i == selectedIndex) {
            attron(A_REVERSE);
        }

        printw("%-*s %-*s %-*s %-*s\n",
               columnWidth1, truncate(files[i][0], columnWidth1).c_str(),
               columnWidth2, truncate(files[i][2], columnWidth2).c_str(),
               columnWidth3, truncate(files[i][3], columnWidth3).c_str(),
               columnWidth4, truncate(files[i][4], columnWidth4).c_str());
        
        if (i == selectedIndex) {
            attroff(A_REVERSE);
        }
    }
    displayHints();
    refresh();
}

void Interface::handleInput(int ch) {
    switch (ch) {
        case KEY_UP:
            if (selectedIndex > 0) {
                --selectedIndex;
            }
            break;
        case KEY_DOWN:
            if (selectedIndex < static_cast<int>(files.size()) - 1) {
                ++selectedIndex;
            }
            break;
        case '\n':
            if (!files.empty()) {
                fm.getFileParts(std::stoi(files[selectedIndex][0]), DOCUMENTS_PATH + files[selectedIndex][2]);
            }
            break;
        case 'q':
            endwin();
            exit(0);
        case 'a':
            promptForFilePath();
            break;
        case 4:
            db.deleteFile(std::stoi(files[selectedIndex][0]));
            LOG("The file has been deleted from the database: " + files[selectedIndex][2] + " Id list: " + files[selectedIndex][1]);
            files.erase(files.begin() + selectedIndex);
            break;
    }
}

void Interface::promptForFilePath() {
    clear();
    printw("Enter path of file to add in cloud: ");
    refresh();

    char filePath[256];
    int index = 0;

    while (true) {
        int ch = getch();

        if (ch == '\n') {
            printf("\n");
            break;
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (index > 0) {
                index--;
                filePath[index] = '\0';
                clear();
                printw("Enter path of file to add in cloud: %s", filePath);
            }
        } else if (index < sizeof(filePath) - 1) {
            filePath[index++] = ch;
            filePath[index] = '\0';
            printw("%c", ch);
            refresh();
        }
    }

    if (index > 0) {
        try {
            fm.sendFileParts(filePath);
            files.push_back(db.getLastFile());
            printw("\nFile added: %s\n", filePath);
        } catch (const std::runtime_error& e) {
            printw("\n%s\n", e.what());
    }
    } else {
        printw("\nNo file path entered.\n");
    }

    printw("Press any key to return to the main screen...");
    refresh();
    
    getch();
    displayFiles();
}

void Interface::displayHints() {
    printw("\n*** Hints: ***\n");
    printw("'Enter' - download file\n");
    printw("'A' - add file to cloud\n");
    printw("'Ctrl + D' - permanently delete\n");
    printw("'Q' - quit\n");
}
