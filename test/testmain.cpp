#include "../src/utils.h"
#include "../src/gapbuffer.h"

int main() {
    GapBuffer<wchar_t> buffer;
    buffer.insert(L'T');
    buffer.insert(L'e');
    buffer.insert(L's');
    buffer.insert(L't');
    
    buffer.moveCursorLeft();
    buffer.moveCursorLeft();
    buffer.moveCursorRight();
    
    std::wcout << buffer.getChar();
    // for (auto elem: buffer) {
    //     std::wcout << elem << L'\n';
    // }
}