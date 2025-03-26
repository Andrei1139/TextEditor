#include "text.h"
// Required for access to wcstok()
#define _CRT_NON_CONFORMING_WCSTOK

Text::Text(wchar_t* initText) {
    cursor.top = cursor.left = 0;
    cursor.right = cursor.left + 1;
    cursor.bottom = cursor.top + CHAR_HEIGHT;

    buffer.insert(GapBuffer<wchar_t>());

    if (!initText)
        return;

    const wchar_t *separators = L"\n";
    auto token = wcstok(initText, separators);

    while (token) {
        // TODO
        // text.push_back(std::wstring(token));
        token = wcstok(NULL, separators);
    }
}

void Text::moveCursorUp() {
    // Cursor is at the top
    if (cursor.top == 0) {
        return;
    }

    cursor.top -= CHAR_HEIGHT - 1;
    buffer.moveCursorLeft();

    // Size of line is larger than max -> move it to the max pos
    if (buffer.getElem()->getSize() > maxX) {
        cursor.left = maxX * CHAR_WIDTH;
        buffer.getElem()->setCursorPos(maxX);
    } else { // Else move it to the end
        cursor.left = buffer.getElem()->getSize() * CHAR_WIDTH;
        buffer.getElem()->setCursorPos(buffer.getElem()->getSize());
    }
}

void Text::moveCursorDown() {
    // Cursor on last row
    if (cursor.top / (CHAR_HEIGHT - 1) >= buffer.getSize() - 1) {
        return;
    }

    cursor.top += CHAR_HEIGHT - 1;
    buffer.moveCursorRight();

    // Line is longer than max -> go to max
    if (buffer.getElem()->getSize() > maxX) {
        cursor.left = maxX * CHAR_WIDTH;
        buffer.getElem()->setCursorPos(maxX);
    } else { // Else go to normal pos
        cursor.left = buffer.getElem()->getSize() * CHAR_WIDTH;
        buffer.getElem()->setCursorPos(buffer.getElem()->getSize());
    }
}

void Text::moveCursorLeft(size_t steps) {
    if (cursor.left > 0) {
        cursor.left -= steps * CHAR_WIDTH;
        buffer.getElem()->moveCursorLeft();
    } else {
        if (cursor.top == 0)
            return;
        buffer.moveCursorLeft();

        cursor.top -= CHAR_HEIGHT - 1;
        cursor.left = buffer.getElem()->getSize() * CHAR_WIDTH;

        buffer.getElem()->setCursorPos(buffer.getElem()->getSize());
    }
    maxX = cursor.left / CHAR_WIDTH;
}

void Text::moveCursorRight(size_t steps) {
    if (cursor.left < buffer.getElem()->getSize() * CHAR_WIDTH) {
        cursor.left += steps * CHAR_WIDTH;
        buffer.getElem()->moveCursorRight();
    } else {
        if (cursor.top / (CHAR_HEIGHT - 1) >= buffer.getSize() - 1) {
            return;
        }
        buffer.moveCursorRight();

        cursor.top += CHAR_HEIGHT - 1;
        cursor.left = 0;

        buffer.getElem()->setCursorPos(0);
    }    
    maxX = cursor.left / CHAR_WIDTH;
}

void Text::handleInput(WPARAM param) {
    switch (param) {
        case L'\n': case L'\r':
            // The cursor must overlap with future position for maintainting relation with text
            handleNewLine();
            break;
        case L'\b':
            handleBackspace();
            break;
        default:
            insert(param);
            if (param == '\t') {
                for (int i = 0; i < 3; ++i)
                    insert(param);
            }
            break;
    }
}

void Text::insert(wchar_t character) {
    cursor.left += CHAR_WIDTH;
    buffer.getElem()->insert(character);
    maxX = buffer.getElem()->getGapStart();
}

void Text::handleNewLine() {
    // The cursor must overlap with future position for maintainting relation with text
    cursor.top += CHAR_HEIGHT - 1;
    cursor.left = 0;

    GapBuffer<wchar_t> *initLine = buffer.getElem();
    buffer.insert(GapBuffer<wchar_t>());
    GapBuffer<wchar_t> *newLine = buffer.getElem();

    wchar_t *character;
    while (initLine->moveCursorRight()) {
        character = initLine->getElem();
        newLine->insert(*character);
        initLine->remove();
    }

    while (newLine->moveCursorLeft()) {}

    maxX = 0;
}

void Text::handleBackspace() {
    if (cursor.left > 0) {
        cursor.left -= CHAR_WIDTH;
        buffer.getElem()->remove();
    } else {
        if (cursor.top == 0)
            return;

        buffer.remove();

        GapBuffer<wchar_t> *line = buffer.getElem();
        cursor.left = line->getSize() * CHAR_WIDTH;
        cursor.top -= CHAR_HEIGHT - 1;
 
        while (line->moveCursorRight()) {}
    }
    --maxX;
}

void Text::handleClick(LPARAM param) {
    auto auxPointStruct = MAKEPOINTS(param);
    auto x = auxPointStruct.x, y = auxPointStruct.y;

    
}