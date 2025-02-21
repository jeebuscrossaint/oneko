#pragma once
#include <SDL2/SDL.h>
#include <memory>
#include <stdexcept>

class Oneko {
public:
    Oneko();
    ~Oneko();

    void run();

private:
    void handle_events();
    void update();
    void render();

    std::unique_ptr<SDL_Window, void(*)(SDL_Window*)> window;
    std::unique_ptr<SDL_Renderer, void(*)(SDL_Renderer*)> renderer;

    bool running = false;
    int mouse_x = 0;
    int mouse_y = 0;
};
