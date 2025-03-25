#include <stdexcept>
#include <stdint.h>
#include "window.h"

static constexpr uint8_t textColor = 255;
static constexpr uint8_t bkColor = 0;

static constexpr COLORREF textRGB = RGB(textColor, textColor, textColor);
static constexpr COLORREF bkRGB = RGB(bkColor, bkColor, bkColor);

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

            // Creating a temporary bitmap and device context for buffering what gets drawn
            HBITMAP tempBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HDC tempHDC = CreateCompatibleDC(hdc);
            SelectObject(tempHDC, tempBuffer);

            // Clear the window (by repainting it white)
            FillRect(tempHDC, &ps.rcPaint, CreateSolidBrush(bkRGB));

            SelectObject(tempHDC, textHandler->getFont());
            SetTextColor(tempHDC, textRGB);
            SetBkColor(tempHDC, bkRGB);
            
            // DrawText(hdc, text.c_str(), text.size(), &rect, DT_LEFT);
            int x = 0, y = 0;
            for (auto& line: textHandler->getBuffer()) {
                for (auto character: line) {
                    TextOut(tempHDC, x, y, &character, 1);
                    x += CHAR_WIDTH;
                }
                TextOut(tempHDC, x, y, L"\r", 1);
                x = 0;
                y += CHAR_HEIGHT - 1;
            }

            // Draw cursor
            if (textHandler->getDisplayCursor())
                FillRect(tempHDC, &cursorRect, CreateSolidBrush(textRGB));

            // Now to copy what got drawn onto the final canvas
            BitBlt(hdc, 0, 0, rect.right, rect.bottom, tempHDC, 0, 0, SRCCOPY);

            // Cleanup
            DeleteObject(tempBuffer);
            DeleteDC(tempHDC);
            
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_KEYDOWN: {
            // Maintain cursor displaying while scrolling
            textHandler->setTypingStatus(true);
            switch (wParam) {
                case VK_LEFT:
                    if (textHandler->isCtrlPressed()) {
                        std::cout << "yay\n";
                    }
                    textHandler->moveCursorLeft();
                    break;
                case VK_RIGHT:
                    textHandler->moveCursorRight();
                    break;
                case VK_UP:
                    textHandler->moveCursorUp();
                    break;
                case VK_DOWN:
                    textHandler->moveCursorDown();
                    break;
                case VK_CONTROL:
                    // std::cout << GetAsyncKeyState(VK_DOWN) << '\n';
                    textHandler->setCtrlPressed(true);
                    break;
            }
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_KEYUP: {
            switch (wParam) {
                case VK_CONTROL:
                    textHandler->setCtrlPressed(false);
                    break;
            }
        }
        case WM_CHAR: {
            // Handle cursor behavior separately
            textHandler->setTypingStatus(true);
            textHandler->handleInput(wParam);

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        // case WM_LBUTTONDOWN: {
        //     for (auto& line: textHandler->getBuffer()) {
        //         std::wcout << line << '\n';
        //     }
            
        //     return 0;
        // }
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
            if (GetMessage(&msg, NULL, 0, 0) <= 0) {
                return;
            }    
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}