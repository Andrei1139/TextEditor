#include <windows.h>
#include <iostream>
#include <fstream>
#include "window.h"

int main(int argc, char* argv[]) {
    #ifndef WIN32
    std::cerr << "Only Windows machines supported\n";
    return -1;
    #endif
    
    if (argc != 2) {
        std::cerr << "Format: TextEditor.exe filepath\n";
        return -1;
    }

    std::ifstream input_file(argv[1]);

    if (!input_file.is_open()) {
        std::cerr << "File could not be opened\n";
        return -1;
    }

    try {
        Window window(L"Nameee");
        window.loop();
    } catch (const std::exception& exception) {
        std::cerr << exception.what();
    }
    return 0;
}