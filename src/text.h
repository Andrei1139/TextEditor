#pragma once
#include <string>
#include <string.h>
#include <vector>
#include <list>
#include <chrono>
#include "gapbuffer.h"
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

        void handleInput(WPARAM param);

        size_t getCursorX() const {return cursor.left;}
        size_t getCursorY() const {return cursor.top;}

        RECT *getCursor() {
            cursor.right = cursor.left + 1;
            cursor.bottom = cursor.top + CHAR_HEIGHT;
            return &cursor;
        }
        GapBuffer<GapBuffer<wchar_t>>& getBuffer() {return buffer;}

        // size_t getSize() const {return buffer.getSize();}
        HFONT getFont() const {return font;}

        void insert(wchar_t character);

        void changeCursorDisplay() {displayCursor = !displayCursor;}
        void changeCursorDisplay(bool status) {displayCursor = status;}
        void setTypingStatus(bool status) {typing = status;}
        bool getDisplayCursor() const {return displayCursor || typing;}
        bool isTyping() const {return typing;}
        void setCtrlPressed(bool pressed) {ctrlPressed = true;}
        bool isCtrlPressed() const {return ctrlPressed;}

    private:
        GapBuffer<GapBuffer<wchar_t>> buffer;
        RECT cursor;
        int maxX = 0;
        HFONT font = (HFONT)(GetStockObject(SYSTEM_FIXED_FONT));

        bool displayCursor = true;
        bool typing = false;
        bool ctrlPressed = false;

        void handleNewLine();
        void handleBackspace();
};