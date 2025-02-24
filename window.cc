#include "oneko_impl.hh"

// Global pointer to Oneko instance
Oneko* g_oneko = nullptr;

void Oneko::create() {
        g_oneko = this;
    
        // Register window class
        WNDCLASSEXW wc = {0};
        wc.cbSize = sizeof(WNDCLASSEXW);
        wc.lpfnWndProc = oneko::WindowProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = L"OnekoWindow";
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        RegisterClassExW(&wc);
    
        // In create():
        hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        L"OnekoWindow",
        L"Oneko",
        WS_POPUP,
        0, 0, 32, 32,
        nullptr, nullptr, hInstance, nullptr
    );
    
    // Just use transparency key for white
    SetLayeredWindowAttributes(hwnd, RGB(255,255,255), 0, LWA_COLORKEY);
    
        // Load images first
        loadImages();
        createTrayIcon();
    
    
        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);
    }

void Oneko::run() {
    MSG msg = {};
    
    // Animation timer
    SetTimer(hwnd, 1, 100, nullptr);  // 10fps

    while(GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK oneko::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch(uMsg) {
        case WM_TIMER:
            if (g_oneko) {
                g_oneko->locateMouseAndAnimateCat();
            }
            return 0;

            case WM_PAINT: {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hwnd, &ps);
                if (g_oneko && !g_oneko->images.empty()) {
                    HDC hdcMem = CreateCompatibleDC(hdc);
                    HBITMAP old = (HBITMAP)SelectObject(hdcMem, g_oneko->images[g_oneko->imageNo]);
                    
                    // Simple BitBlt with alpha
                    BLENDFUNCTION blend = { 0 };
                    blend.BlendOp = AC_SRC_OVER;
                    blend.SourceConstantAlpha = 255;
                    blend.AlphaFormat = AC_SRC_ALPHA;
                    
                    AlphaBlend(hdc, 0, 0, 32, 32,
                               hdcMem, 0, 0, 32, 32,
                               blend);
                    
                    SelectObject(hdcMem, old);
                    DeleteDC(hdcMem);
                }
                EndPaint(hwnd, &ps);
                return 0;
            }

        case WM_USER + 1:
            if (lParam == WM_RBUTTONUP) {
                POINT pt;
                GetCursorPos(&pt);
                HMENU hMenu = CreatePopupMenu();
                InsertMenuW(hMenu, 0, MF_BYPOSITION | MF_STRING, 1, L"Exit");
                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY,
                                      pt.x, pt.y, 0, hwnd, nullptr);
                DestroyMenu(hMenu);
                
                if (cmd == 1) {
                    DestroyWindow(hwnd);
                }
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}