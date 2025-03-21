#pragma once
#include <string>
#include <string.h>
#include <vector>
#include <chrono>
#include "utils.h"

constexpr size_t CHAR_WIDTH = 8;
constexpr size_t CHAR_HEIGHT = 16;

class Text {
    public:
        Text(wchar_t* initText = nullptr);

        void moveCursorUp();
        void moveCursorDown();
        void moveCursorLeft(size_t steps = 1);
        void moveCursorRight(size_t steps = 1);

        void detCursorBehav(WPARAM param);

        size_t getCursorX() const {return cursor.left;}
        size_t getCursorY() const {return cursor.top;}

        RECT *getCursor() {
            cursor.right = cursor.left + 1;
            cursor.bottom = cursor.top + CHAR_HEIGHT;
            return &cursor;
        }
        size_t getSize() const {return text.size();}
        HFONT getFont() const {return font;}

        void changeCursorDisplay() {displayCursor = !displayCursor;}
        void changeCursorDisplay(bool status) {displayCursor = status;}
        void setTypingStatus(bool status) {typing = status;}
        bool getDisplayCursor() const {return displayCursor || typing;}
        bool isTyping() const {return typing;}

    private:
        std::vector<std::wstring> text;
        RECT cursor;
        HFONT font = (HFONT)(GetStockObject(SYSTEM_FIXED_FONT));

        bool displayCursor = true;
        bool typing = false;
};