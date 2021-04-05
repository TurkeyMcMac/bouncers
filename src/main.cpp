#include "conf.hpp"
#include "simulate.hpp"
#include <SDL2/SDL.h>

using namespace bouncers;

int main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
            "SDL initialization failed; %s\n", SDL_GetError());
        goto error_sdl_init;
    }
    window = SDL_CreateWindow("Bouncers", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, conf::WINDOW_WIDTH, conf::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
            "SDL window creation failed; %s\n", SDL_GetError());
        goto error_create_window;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        SDL_LogCritical(SDL_LOG_CATEGORY_ERROR,
            "SDL renderer creation failed; %s\n", SDL_GetError());
        goto error_create_surface;
    }
    simulate(renderer, argc >= 2 ? (unsigned)atoi(argv[1]) : 1337U);
    status = EXIT_SUCCESS;
    SDL_DestroyRenderer(renderer);
error_create_surface:
    SDL_DestroyWindow(window);
error_create_window:
    SDL_Quit();
error_sdl_init:
    return status;
}
