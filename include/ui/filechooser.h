#pragma once

#include <string>
#include <vector>

class FileChooser {
public:
    FileChooser(std::string directory, std::vector<std::string> extensions, bool canQuit);

    std::string getDirectory();
    void setDirectory(std::string directory);
    char* startFileChooser();

private:
    void updateScrollDown();
    void updateScrollUp();

    void refreshContents();
    void redrawChooser();
    bool updateChooser(char** result);

    std::string directory;
    std::vector<std::string> extensions;
    bool canQuit;

    std::vector<std::string> filenames;
    std::vector<int> flags;
    int selection = 0;
    int filesPerPage = 24;
    int numFiles = 0;
    int scrollY = 0;
    std::string matchFile = "";
};
