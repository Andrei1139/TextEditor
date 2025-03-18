#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>

class Window {
    public:
        Window(LPCTSTR window_name = L"Default name");
        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        void loop();

    private:
        LPCTSTR window_name;
        HWND window_handle;
        LRESULT CALLBACK (*procedure)(HWND, UINT, WPARAM, LPARAM);

        std::string text{};
};