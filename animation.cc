#include "oneko_impl.hh"

  // Add this method implementation
  void Oneko::locateMouseAndAnimateCat() {
        POINT pt;
        GetCursorPos(&pt);
        
        // Calculate distance between cat and cursor
        int dx = pt.x - (ox + offsetX);
        int dy = pt.y - (oy + offsetY);
        double dist = sqrt(dx*dx + dy*dy);
        
        // Safety check - ensure we have loaded images
        if (images.empty()) {
            return;
        }
        
        // Update cat position based on distance
        if (dist > triggerDistance) {
            mouseMoved = true;
            
            // Move cat towards cursor
            if (dist > runDistancePerFrame) {
                // Avoid division by zero
                if (dist != 0) {
                    double scale = runDistancePerFrame / dist;
                    ox += static_cast<int>(dx * scale);
                    oy += static_cast<int>(dy * scale);
                }
            } else {
                ox = pt.x - offsetX;
                oy = pt.y - offsetY;
            }
            
            // Update window position
            SetWindowPos(hwnd, HWND_TOPMOST, ox, oy, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
            
            // Update animation frame with bounds check
            imageNo = (imageNo + 1) % images.size();
            
            // Force redraw
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }