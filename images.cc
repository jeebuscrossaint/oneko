#include "oneko_impl.hh"

void Oneko::loadImages() {
        // Initialize GDI+
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr) != Gdiplus::Ok) {
            return;
        }
    
        // Load numbered PNG files from 1 to 32
        for (int i = 1; i <= 32; i++) {
            wchar_t filename[MAX_PATH];
            swprintf(filename, MAX_PATH, L"gif/%d.GIF", i);  // Back to .GIF
            HBITMAP hBitmap = LoadGifAsBitmap(filename);
            if (hBitmap) {
                images.push_back(hBitmap);
            }
        }
    
        // If no images loaded, try to load at least one default image
        if (images.empty()) {
            HBITMAP hBitmap = LoadGifAsBitmap(L"gif/1.png");  // Changed to .png
            if (hBitmap) {
                images.push_back(hBitmap);
            }
        }
    }
    
    HBITMAP Oneko::LoadGifAsBitmap(const wchar_t* filename) {
        // Load image directly
        Gdiplus::Bitmap* bitmap = Gdiplus::Bitmap::FromFile(filename);
        if (!bitmap) return nullptr;
    
        // Create direct bitmap copy with no transformations
        Gdiplus::Bitmap* conv = new Gdiplus::Bitmap(32, 32, PixelFormat32bppPARGB);
        Gdiplus::Graphics* g = Gdiplus::Graphics::FromImage(conv);
        
        // Clear background
        g->Clear(Gdiplus::Color::Transparent);
        
        // Draw image directly
        g->DrawImage(bitmap, 0, 0, 32, 32);
        
        delete g;
        delete bitmap;
    
        // Convert to HBITMAP
        HBITMAP hBitmap = nullptr;
        conv->GetHBITMAP(Gdiplus::Color::Transparent, &hBitmap);
        delete conv;
    
        return hBitmap;
    }