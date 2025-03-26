#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include "utils.h"
#include "text.h"

class Window {
    public:
        Window(LPCTSTR window_name = L"Default name");
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void loop();
        Text& getText() {return text;}
        int horizontalShift = 0, verticalShift = 0;
    private:
        LPCTSTR window_name;
        HWND window_handle;
        LRESULT CALLBACK (*procedure)(HWND, UINT, WPARAM, LPARAM);
        Text text;
};