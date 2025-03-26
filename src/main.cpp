#include <fstream>
#include "utils.h"
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

    std::wifstream input_file(argv[1]);

    if (!input_file.is_open()) {
        std::cerr << "File could not be opened\n";
        return -1;
    }

    try {
        Window window(L"Name", &input_file);
        window.loop();
        std::wofstream output(argv[1]);

        if (!output.is_open()) {
            std::cerr << "Output cold not be written\n";
            return -1;
        }

        TextParser::writeText(window.getText(), output);
    } catch (const std::exception& exception) {
        std::cerr << "Exception: " << exception.what();
    }
    return 0;
}