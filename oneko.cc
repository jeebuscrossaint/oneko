#include "oneko.hh"

Oneko::Oneko()
    : window(nullptr, SDL_DestroyWindow)
    , renderer(nullptr, SDL_DestroyRenderer)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error(SDL_GetError());
    }

    window.reset(SDL_CreateWindow(
        "Oneko",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        32, 32,  // Initial size
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP
    ));

    if (!window) {
        throw std::runtime_error(SDL_GetError());
    }

    renderer.reset(SDL_CreateRenderer(
        window.get(), -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    ));

    if (!renderer) {
        throw std::runtime_error(SDL_GetError());
    }
}

Oneko::~Oneko() {
    SDL_Quit();
}

void Oneko::run() {
    running = true;
    while (running) {
        handle_events();
        update();
        render();
    }
}

void Oneko::handle_events() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEMOTION:
                SDL_GetGlobalMouseState(&mouse_x, &mouse_y);
                break;
        }
    }
}

void Oneko::update() {
    // TODO: Update cat position based on mouse position
}

void Oneko::render() {
    SDL_SetRenderDrawColor(renderer.get(), 255, 0, 255, 255);
    SDL_RenderClear(renderer.get());

    // TODO: Draw cat sprite

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
