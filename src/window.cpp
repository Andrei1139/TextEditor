#include <stdexcept>
#include "window.h"

static std::wstring text{};

static constexpr COLORREF textColor = RGB(255, 255, 255);
static constexpr COLORREF bkColor = RGB(0, 0, 0);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // For class access, since the function doesnt allow additionals args
    Text *textHandler = reinterpret_cast<Text *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            tagRECT rect, cursorRect = *(textHandler->getCursor());

            GetClientRect(hwnd, &rect);
            // Clear the window (by repainting it white)
            FillRect(hdc, &ps.rcPaint, CreateSolidBrush(bkColor));

            SelectObject(hdc, textHandler->getFont());
            SetTextColor(hdc, textColor);
            SetBkColor(hdc, bkColor);
            
            DrawText(hdc, text.c_str(), text.size(), &rect, DT_LEFT);

            // Draw cursor
            if (textHandler->getDisplayCursor())
                FillRect(hdc, &cursorRect, CreateSolidBrush(textColor));
            EndPaint(hwnd, &ps);
            
            return 0;
        }
        case WM_CHAR: {
            // Handle cursor behavior separately
            textHandler->setTypingStatus(true);
            textHandler->detCursorBehav(wParam);
            // Handle backspace pressing (but only when there are characters on the screen)
            if (wParam == L'\b') {
                if (text.size() > 0)
                    text.pop_back();    
            } else {
                text += wParam;
            }
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
    }
    // Rest of messages get handled by the default procedure
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void register_class(WNDPROC procedure, LPCWSTR class_name) {
    WNDCLASS wndclass = {};
    wndclass.lpfnWndProc = procedure;
    wndclass.hInstance = GetModuleHandle(nullptr);
    wndclass.hCursor = LoadCursor(nullptr, IDC_IBEAM);
    wndclass.lpszClassName = class_name;
    wndclass.hbrBackground = CreateSolidBrush(bkColor);

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

    // For class access outside the class
    SetWindowLongPtr(window_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&text)); 

    ShowWindow(window_handle, SW_SHOW);
}

void Window::loop() {
    using namespace std::chrono;
    constexpr milliseconds oneFlicker(1000/2);
    
    MSG msg = {};
    auto initTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    while (true) {
        // Handle text cursor display logic
        auto currTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

        if (text.isTyping()) {
            initTime = currTime;
            text.setTypingStatus(false);
            text.changeCursorDisplay(true);
        }

        if (currTime - initTime >= oneFlicker) {
            text.changeCursorDisplay();
            initTime = currTime;
            RedrawWindow(window_handle, NULL, NULL, RDW_INVALIDATE);
        }

        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) != 0) {
            if (GetMessage(&msg, NULL, 0, 0) <= 0)
                return;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}