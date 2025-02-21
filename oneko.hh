#pragma once

// C++ STD
#include <memory>
#include <stdexcept>

// SDL2
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>

// Wayland / wlroots
#include <wayland-client.h>
#include "layer_shell_fixes.h"

// Include our generated headers from spritesheet.py
#include "spritesheet.h"
#include "frame_info.h"

struct LayerSurfaceData {
    struct zwlr_layer_shell_v1* layer_shell = nullptr;
    uint32_t name = 0;
    uint32_t version = 0;
};

static void registry_global(void* data, struct wl_registry* registry,
                          uint32_t name, const char* interface,
                          uint32_t version);
static void registry_global_remove(void* data, struct wl_registry* registry,
                                 uint32_t name);

class Oneko {
public:
    Oneko();
    ~Oneko();

    void run();

private:
    static constexpr int SPRITE_WIDTH = 32;
    static constexpr int SPRITE_HEIGHT = 32;

    void handle_events();
    void update();
    void render();
    void load_sprites();

    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> window;
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> renderer;
    std::unique_ptr<SDL_Texture, void(*)(SDL_Texture*)> sprite_sheet;

    SDL_Rect sprite_rect{0, 0, SPRITE_WIDTH, SPRITE_HEIGHT};
    int current_frame = 0;
    int current_state = 0;

    bool running = false;
    int mouse_x = 0;
    int mouse_y = 0;
    int cat_x = 0;
    int cat_y = 0;

    // Wayland-specific members
        struct wl_display* display = nullptr;
        struct wl_surface* surface = nullptr;
        struct zwlr_layer_shell_v1* layer_shell = nullptr;
        struct zwlr_layer_surface_v1* layer_surface = nullptr;

        static void layer_surface_configure(
            void *data,
            struct zwlr_layer_surface_v1 *surface,
            uint32_t serial,
            uint32_t width,
            uint32_t height
        );

        static void layer_surface_closed(
            void *data,
            struct zwlr_layer_surface_v1 *surface
        );
};
