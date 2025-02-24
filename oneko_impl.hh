#pragma once
#include "oneko.hh"
#include <gdiplus.h>

// Global pointer to Oneko instance for window procedure
extern Oneko* g_oneko;

// Internal helper functions
namespace oneko {
    // Window procedure callback
    LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
}