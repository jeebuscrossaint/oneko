#include "oneko.hh"
#include <stdexcept>

void registry_global(void* data, struct wl_registry* registry,
                    uint32_t name, const char* interface,
                    uint32_t version) {
    auto* shell_data = static_cast<LayerSurfaceData*>(data);
    if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
        shell_data->layer_shell =
            static_cast<struct zwlr_layer_shell_v1*>(
                wl_registry_bind(registry, name,
                               &zwlr_layer_shell_v1_interface,
                               std::min(version, 1u)));
        shell_data->name = name;
        shell_data->version = version;
    }
}

void registry_global_remove(void* data, struct wl_registry* registry,
                          uint32_t name) {
    // Handle removal if needed
}

Oneko::Oneko()
    : window(nullptr, SDL_DestroyWindow)
    , renderer(nullptr, SDL_DestroyRenderer)
    , sprite_sheet(nullptr, SDL_DestroyTexture)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
    }

    // Force Wayland backend
    SDL_SetHint(SDL_HINT_VIDEODRIVER, "wayland");

    window.reset(SDL_CreateWindow(
        "Oneko",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SPRITE_WIDTH, SPRITE_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP
    ));

    if (!window) {
        throw std::runtime_error(SDL_GetError());
    }

    // Get Wayland display and surface
    SDL_SysWMinfo wm_info = {};  // Zero-initialize the struct
        SDL_VERSION(&wm_info.version);
        if (!SDL_GetWindowWMInfo(window.get(), &wm_info)) {
            throw std::runtime_error(SDL_GetError());
        }

        if (wm_info.subsystem != SDL_SYSWM_WAYLAND) {
            throw std::runtime_error("Not running under Wayland");
        }

        display = wm_info.info.wl.display;
        surface = wm_info.info.wl.surface;

        if (!display || !surface) {
            throw std::runtime_error("Couldn't get Wayland display or surface");
        }

        // Get layer shell
            LayerSurfaceData shell_data;
            struct wl_registry* registry = wl_display_get_registry(display);
            if (!registry) {
                throw std::runtime_error("Couldn't get Wayland registry");
            }

            struct wl_registry_listener registry_listener = {
                .global = registry_global,
                .global_remove = registry_global_remove,
            };

            wl_registry_add_listener(registry, &registry_listener, &shell_data);
            wl_display_roundtrip(display);

            if (!shell_data.layer_shell) {
                throw std::runtime_error("Compositor doesn't support layer shell");
            }

            layer_shell = shell_data.layer_shell;

    if (!layer_shell) {
        throw std::runtime_error("Couldn't create layer shell");
    }

    // Create layer surface
        layer_surface = create_layer_surface(  // Use create_layer_surface as defined in the header
            layer_shell,
            surface,
            nullptr,
            ZWLR_LAYER_SHELL_V1_LAYER_OVERLAY,
            "oneko-app"
        );

    if (!layer_surface) {
        throw std::runtime_error("Couldn't create layer surface");
    }

    // Configure layer surface
    zwlr_layer_surface_v1_set_size(layer_surface, SPRITE_WIDTH, SPRITE_HEIGHT);
    zwlr_layer_surface_v1_set_anchor(layer_surface, 0);
    zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, -1);
    zwlr_layer_surface_v1_set_keyboard_interactivity(
        layer_surface,
        ZWLR_LAYER_SURFACE_V1_KEYBOARD_INTERACTIVITY_NONE
    );

    // Set up layer surface listener
    static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
        .configure = layer_surface_configure,
        .closed = layer_surface_closed,
    };

    zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener, this);
    wl_surface_commit(surface);

    renderer.reset(SDL_CreateRenderer(
        window.get(), -1,
        SDL_RENDERER_ACCELERATED
    ));

    if (!renderer) {
        throw std::runtime_error(SDL_GetError());
    }

    SDL_SetRenderDrawBlendMode(renderer.get(), SDL_BLENDMODE_BLEND);
    load_sprites();
}

// Add these callback implementations
void Oneko::layer_surface_configure(void *data,
    struct zwlr_layer_surface_v1 *surface,
    uint32_t serial,
    uint32_t width,
    uint32_t height)
{
    zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void Oneko::layer_surface_closed(void *data,
    struct zwlr_layer_surface_v1 *surface)
{
    Oneko* oneko = static_cast<Oneko*>(data);
    oneko->running = false;
}

void Oneko::load_sprites() {
    // Create an SDL_RWops from our embedded sprite sheet data
    SDL_RWops* rw = SDL_RWFromMem(spritesheet_png, spritesheet_png_len);
    if (!rw) {
        throw std::runtime_error(SDL_GetError());
    }

    // Load the surface from the RWops
    SDL_Surface* surface = IMG_LoadPNG_RW(rw);
    SDL_RWclose(rw);

    if (!surface) {
        throw std::runtime_error(IMG_GetError());
    }

    // Create texture from surface
    sprite_sheet.reset(SDL_CreateTextureFromSurface(renderer.get(), surface));
    SDL_FreeSurface(surface);

    if (!sprite_sheet) {
        throw std::runtime_error(SDL_GetError());
    }

    // Set texture blend mode for transparency
    SDL_SetTextureBlendMode(sprite_sheet.get(), SDL_BLENDMODE_BLEND);
}

Oneko::~Oneko() {
    if (layer_surface) {
        zwlr_layer_surface_v1_destroy(layer_surface);
    }
    if (layer_shell) {
        zwlr_layer_shell_v1_destroy(layer_shell);
    }
    SDL_Quit();
}

void Oneko::run() {
    running = true;

    // Start in center of screen
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    cat_x = dm.w / 2;
    cat_y = dm.h / 2;

    Uint32 last_time = SDL_GetTicks();
    const Uint32 frame_delay = 16;  // ~60 FPS

    while (running) {
        Uint32 current_time = SDL_GetTicks();
        Uint32 delta = current_time - last_time;

        if (delta >= frame_delay) {
            handle_events();
            update();
            render();
            last_time = current_time;
        } else {
            SDL_Delay(1);
        }
    }
}

void Oneko::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        }
    }
    SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
}

void Oneko::update() {
    int dx = mouse_x - cat_x;
    int dy = mouse_y - cat_y;
    float distance = sqrt(dx*dx + dy*dy);

    if (distance < 16) {
        current_state = 0; // sitting
        current_frame = 0; // reset animation when sitting
    } else {
        float speed = 8.0f;  // Increased speed
        cat_x += (dx / distance) * speed;
        cat_y += (dy / distance) * speed;

        // Set direction based on movement
        if (abs(dx) > abs(dy)) {
            if (dx > 0) current_state = 2; // right
            else current_state = 1; // left
        } else {
            if (dy > 0) current_state = 4; // down
            else current_state = 3; // up
        }

        // Update animation frame every few frames
        if (SDL_GetTicks() % 100 < 50) {  // Slower animation
            current_frame = (current_frame + 1) % 4;
        }
    }

    SDL_SetWindowPosition(window.get(), cat_x - SPRITE_WIDTH/2, cat_y - SPRITE_HEIGHT/2);
}

void Oneko::render() {
    // Clear with transparent background
    SDL_SetRenderDrawColor(renderer.get(), 0, 0, 0, 0);
    SDL_RenderClear(renderer.get());

    // Calculate source rectangle for current frame
    SDL_Rect src = {
        current_frame * SPRITE_WIDTH,
        current_state * SPRITE_HEIGHT,
        SPRITE_WIDTH,
        SPRITE_HEIGHT
    };

    // Calculate destination rectangle
    SDL_Rect dst = {0, 0, SPRITE_WIDTH, SPRITE_HEIGHT};

    // Render current frame
    SDL_RenderCopy(renderer.get(), sprite_sheet.get(), &src, &dst);

    SDL_RenderPresent(renderer.get());
}

int main(int argc, char* argv[]) {
    try {
        Oneko oneko;
        oneko.run();
        return 0;
    } catch (const std::exception& e) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Error",
            e.what(),
            nullptr);
        return 1;
    }
}
