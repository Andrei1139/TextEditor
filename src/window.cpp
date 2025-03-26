#include <stdexcept>
#include <stdint.h>
#include "window.h"

static constexpr uint8_t textColor = 255;
static constexpr uint8_t bkColor = 0;

static constexpr COLORREF textRGB = RGB(textColor, textColor, textColor);
static constexpr COLORREF bkRGB = RGB(bkColor, bkColor, bkColor);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // For class access, since the function doesnt allow additionals args
    Window *windowInstance = reinterpret_cast<Window *>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            // info.cbSize = sizeof(SCROLLINFO);
            // info.fMask = SIF_RANGE;
            // info.nMin = 0;
            // info.nMax = 100;
            // info.nPage = 5;
            // info.nPos = 0;
            
            // GetScrollInfo(hwnd, SB_HORZ, &info);

            // std::cout << info.nMin << ' ' << info.nMax << ' ' << info.nPage << ' ' << info.nPos << '\n';

    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            tagRECT rect, cursorRect = *(windowInstance->getText().getCursor());
            GetClientRect(hwnd, &rect);

            // Creating a temporary bitmap and device context for buffering what gets drawn
            HBITMAP tempBuffer = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
            HDC tempHDC = CreateCompatibleDC(hdc);
            SelectObject(tempHDC, tempBuffer);

            // Clear the window (by repainting it white)
            FillRect(tempHDC, &ps.rcPaint, CreateSolidBrush(bkRGB));

            SelectObject(tempHDC, windowInstance->getText().getFont());
            SetTextColor(tempHDC, textRGB);
            SetBkColor(tempHDC, bkRGB);
            
            // DrawText(hdc, text.c_str(), text.size(), &rect, DT_LEFT);
            int x = -windowInstance->horizontalShift * CHAR_WIDTH;
            int y = -windowInstance->verticalShift * CHAR_HEIGHT;
            for (auto& line: windowInstance->getText().getBuffer()) {
                for (auto character: line) {
                    TextOut(tempHDC, x, y, &character, 1);
                    x += CHAR_WIDTH;
                }
                TextOut(tempHDC, x, y, L"\r", 1);
                x = -windowInstance->horizontalShift * CHAR_WIDTH;
                y += CHAR_HEIGHT - 1;
            }

            // Draw cursor
            auto tempCursorRect = cursorRect;
            tempCursorRect.left -= windowInstance->horizontalShift * CHAR_WIDTH;
            tempCursorRect.right = tempCursorRect.left + 1;
            tempCursorRect.top -= windowInstance->verticalShift *CHAR_HEIGHT;
            tempCursorRect.bottom = tempCursorRect.top + 16;
            if (windowInstance->getText().getDisplayCursor())
                FillRect(tempHDC, &tempCursorRect, CreateSolidBrush(textRGB));

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
            windowInstance->getText().setTypingStatus(true);
            switch (wParam) {
                case VK_LEFT:
                    windowInstance->getText().moveCursorLeft();
                    break;
                case VK_RIGHT:
                    windowInstance->getText().moveCursorRight();
                    break;
                case VK_UP:
                    windowInstance->getText().moveCursorUp();
                    break;
                case VK_DOWN:
                    windowInstance->getText().moveCursorDown();
                    break;
                case VK_CONTROL:
                    // std::cout << GetAsyncKeyState(VK_DOWN) << '\n';
                    windowInstance->getText().setCtrlPressed(true);
                    break;
                }
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_KEYUP: {
            switch (wParam) {
                case VK_CONTROL:
                    windowInstance->getText().setCtrlPressed(false);
                    break;
            }
            return 0;
        }
        case WM_CHAR: {
            // Handle cursor behavior separately
            windowInstance->getText().setTypingStatus(true);
            windowInstance->getText().handleInput(wParam);

            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_HSCROLL: {
            SCROLLINFO info;
            info.cbSize = sizeof(SCROLLINFO);
            info.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_HORZ, &info);

            switch(LOWORD(wParam)) {
                case SB_THUMBPOSITION: case SB_THUMBTRACK:
                    info.nPos = HIWORD(wParam);
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
                    break;

                case SB_LINELEFT:
                    info.nPos = (info.nPos == 0) ? 0 : --info.nPos;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
                    break;

                case SB_LINERIGHT:
                    info.nPos = (info.nPos == info.nMax) ? info.nMax : ++info.nPos;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
                    break;    

                case SB_PAGELEFT:
                    info.nPos = (info.nPos - info.nPage <= 0) ? 0 : info.nPos - info.nPage;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
                    break;
                    
                case SB_PAGERIGHT:
                    info.nPos = (info.nPos + info.nPage >= info.nMax) ? info.nMax - info.nPage : info.nPos + info.nPage;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_HORZ, &info, TRUE);
                    break;
            }

            windowInstance->horizontalShift = info.nPos;
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            return 0;
        }
        case WM_VSCROLL: {
            SCROLLINFO info;
            info.cbSize = sizeof(SCROLLINFO);
            info.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_VERT, &info);

            switch(LOWORD(wParam)) {
                case SB_THUMBPOSITION: case SB_THUMBTRACK:
                    info.nPos = HIWORD(wParam);
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
                    break;

                case SB_LINEUP:
                    info.nPos = (info.nPos == 0) ? 0 : --info.nPos;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
                    break;

                case SB_LINEDOWN:
                    info.nPos = (info.nPos == info.nMax) ? info.nMax : ++info.nPos;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
                    break;    

                case SB_PAGEUP:
                    info.nPos = (info.nPos - info.nPage <= 0) ? 0 : info.nPos - info.nPage;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
                    break;
                    
                case SB_PAGEDOWN:
                    info.nPos = (info.nPos + info.nPage >= info.nMax) ? info.nMax - info.nPage : info.nPos + info.nPage;
                    info.fMask = SIF_POS;
                    SetScrollInfo(hwnd, SB_VERT, &info, TRUE);
                    break;
            }

            windowInstance->verticalShift = info.nPos;
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

Window::Window(LPCTSTR window_name_arg, std::wistream* input_file)
: window_name{window_name_arg},
  procedure{WindowProc},
  text{TextParser::readText(*input_file)} {
    register_class(procedure, L"Window Class");

    window_handle = CreateWindowEx(
        0,
        L"Window Class",
        L"Tests",
        WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, GetModuleHandle(nullptr), NULL
      );

    if (window_handle == NULL) {
        throw std::runtime_error{"Could dont create window"};
    }

    // Setting up the scroll bars
    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLBARINFO);
    info.fMask = SIF_PAGE | SIF_RANGE;
    info.nMin = 0;
    info.nMax = 100;
    info.nPage = 20;
    SetScrollInfo(window_handle, SB_HORZ, &info, TRUE);
    SetScrollInfo(window_handle, SB_VERT, &info, TRUE);

    // For class access outside the class
    SetWindowLongPtr(window_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this)); 

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