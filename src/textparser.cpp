#include "textparser.h"

Text TextParser::readText(std::wistream& input) {
    Text text;
    wchar_t character;
    while ((character = input.get()) != (wchar_t)EOF) {
        text.handleInput(character);
    }

    return text;
}

void TextParser::writeText(Text& text, std::wostream& output) {
    auto& textBuffer = text.getBuffer();

    for (auto& line: textBuffer) {
        for (auto character: line) {
            output << character;
        }
        output << "\n";
    }
}