#include "oneko_impl.hh"

void Oneko::createTrayIcon() {
        ZeroMemory(&nid, sizeof(NOTIFYICONDATAW));
        nid.cbSize = sizeof(NOTIFYICONDATAW);
        nid.hWnd = hwnd;
        nid.uID = 1;
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_USER + 1;
        
        // Load first image as icon
        if (!images.empty()) {
            HICON hIcon = CreateIconFromBitmap(images[0]);
            nid.hIcon = hIcon;
        }
        
        // Copy tooltip text (wide-char)
        lstrcpynW(nid.szTip, L"Oneko", ARRAYSIZE(nid.szTip));
        Shell_NotifyIconW(NIM_ADD, &nid);
    }

HICON Oneko::CreateIconFromBitmap(HBITMAP hBitmap) {
    BITMAP bm;
    GetObject(hBitmap, sizeof(bm), &bm);
    
    HICON hIcon = nullptr;
    ICONINFO ii = {0};
    ii.fIcon = TRUE;
    ii.xHotspot = 0;
    ii.yHotspot = 0;
    ii.hbmMask = CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, nullptr);
    ii.hbmColor = hBitmap;
    
    hIcon = CreateIconIndirect(&ii);
    DeleteObject(ii.hbmMask);
    
    return hIcon;
}