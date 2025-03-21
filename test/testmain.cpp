#include "../src/utils.h"
#include "../src/gapbuffer.h"

int main() {
    GapBuffer buffer;
    buffer.insert(L'T');
    buffer.insert(L'e');
    buffer.insert(L's');
    buffer.insert(L't');
    
    buffer.moveCursorLeft();
    buffer.moveCursorLeft();
    buffer.moveCursorRight();
    
    for (auto elem: buffer) {
        std::wcout << elem << L'\n';
    }
}