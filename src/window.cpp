#include <windows.h>
#include <stdexcept>
#include <iostream>
#include "window.h"

std::wstring text{};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            // HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            // FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_GRADIENTACTIVECAPTION + 1));

            // EndPaint(hwnd, &ps);
            
            HDC device_context = GetDC(hwnd);
            // SetBkColor(device_context, RGB(0, 0, 0));
            tagRECT rect;
            GetWindowRect(hwnd, &rect);

            rect.bottom -= rect.top;
            rect.right -= rect.left;
            rect.top = rect.left = 0;

            DrawText(device_context, text.c_str(), text.size(), &rect, DT_LEFT);

            std::cout << rect.top << ' ' << rect.left << ' ' << rect.bottom << ' ' << rect.right << '\n';
            
            return 0;
        }
        case WM_KEYDOWN: {
            text.append(L"t");
            return 0;
        }
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void register_class(WNDPROC procedure, LPCWSTR class_name) {
    WNDCLASS wndclass = {};
    wndclass.lpfnWndProc = procedure;
    wndclass.hInstance = GetModuleHandle(nullptr);
    wndclass.hCursor = LoadCursor(nullptr, IDC_IBEAM);
    wndclass.lpszClassName = class_name;
    wndclass.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
    // wndclass.hIcon = LoadIcon(wndclass.hInstance, )

    RegisterClass(&wndclass);
}

Window::Window(LPCTSTR window_name_arg)
: window_name{window_name_arg},
  procedure{WindowProc} {
    register_class(procedure, L"Window Class");

    window_handle = CreateWindowEx(
        0,
        L"Window Class",
        L"Tests",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(nullptr), NULL
      );

    if (window_handle == NULL) {
        throw std::runtime_error{"Could dont create window"};
    }

    ShowWindow(window_handle, SW_SHOW);
}

void Window::loop() {
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}