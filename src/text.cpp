#include "text.h"
// Required for access to wcstok()
#define _CRT_NON_CONFORMING_WCSTOK

Text::Text(wchar_t* initText) {
    cursor.top = cursor.left = 0;
    cursor.right = cursor.left + 1;
    cursor.bottom = cursor.top + CHAR_HEIGHT;

    if (!initText)
        return;

    const wchar_t *separators = L"\n";
    auto token = wcstok(initText, separators);

    while (token) {
        text.push_back(std::wstring(token));
        token = wcstok(NULL, separators);
    }
}

void Text::moveCursorUp() {
    return;
}

void Text::moveCursorDown() {
    return;
}

void Text::moveCursorLeft(size_t steps) {
    if (cursor.left > 0)
        cursor.left -= steps * CHAR_WIDTH;
    return;
}

void Text::moveCursorRight(size_t steps) {
    cursor.left += steps * CHAR_WIDTH;
    return;
}

void Text::detCursorBehav(WPARAM param) {
    switch (param) {
        case L'\n': case L'\r':
            // The cursor must overlap with future position for maintainting relation with text
            cursor.top += CHAR_HEIGHT - 1;
            cursor.left = 0;
            break;
        case L'\b':
            moveCursorLeft();
            break;
        default:
            moveCursorRight();
            break;
    }
}