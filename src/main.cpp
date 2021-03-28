#include "Agent.hpp"
#include "scalar.hpp"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>
#include <random>

using namespace bouncers;

static void draw_circle(
    SDL_Renderer* renderer, scalar x, scalar y, scalar radius)
{
    static const struct {
        scalar x, y;
    } offsets[] = {
        { 1, 0 },
        { .92387953251127686560, .38268343236511364957 },
        { .70710678118651097377, .70710678118658407502 },
        { .38268343236501813819, .92387953251131642770 },
        { 0, 1 },
        { -.38268343236520916094, .92387953251123730349 },
        { -.70710678118665717626, .70710678118643787253 },
        { -.92387953251135598981, .38268343236492262682 },
        { -1, 0 },
        { -.92387953251119774138, -.38268343236530467232 },
        { -.70710678118636477129, -.70710678118673027750 },
        { -.38268343236482711544, -.92387953251139555192 },
        { 0, -1 },
        { .38268343236540018370, -.92387953251115817927 },
        { .70710678118680337874, -.70710678118629167005 },
        { .92387953251143511403, -.38268343236473160406 },
        { 1, 0 },
    };
    static const int N_POINTS = sizeof(offsets) / sizeof(*offsets);
    SDL_Point points[N_POINTS];
    for (int i = 0; i < N_POINTS; ++i) {
        points[i].x = std::round(x + offsets[i].x * radius);
        points[i].y = std::round(y + offsets[i].y * radius);
    }
    SDL_RenderDrawLines(renderer, points, N_POINTS);
}

static void draw_body(SDL_Renderer* renderer, const Body& body, scalar radius,
    scalar screen_scale, int screen_origin_x, int screen_origin_y)
{
    scalar screen_x = body.x * screen_scale + screen_origin_x;
    scalar screen_y = body.y * screen_scale + screen_origin_y;
    scalar screen_radius = radius * screen_scale;
    draw_circle(renderer, screen_x, screen_y, screen_radius);
    scalar screen_head_x = screen_x + std::cos(body.ang) * screen_radius;
    scalar screen_head_y = screen_y + std::sin(body.ang) * screen_radius;
    SDL_RenderDrawLine(renderer, std::round(screen_x), std::round(screen_y),
        std::round(screen_head_x), std::round(screen_head_y));
}

static void simulate(SDL_Renderer* renderer)
{
    static const scalar RADIUS = 40;
    static const int N_AGENTS = 4;
    Agent agents[N_AGENTS];
    std::random_device rand_dev;
    std::minstd_rand rand(rand_dev());
    std::uniform_real_distribution<scalar> real_dis(-1, 1);
    auto randomize
        = [rand, real_dis](scalar& w) mutable { w = real_dis(rand); };
    for (int i = 0; i < N_AGENTS; ++i) {
        for (int j = 0; j < Agent::MEMORY_SIZE; ++j) {
            agents[i].memory[j] = 0;
        }
        agents[i].self_brain.for_each_weight(randomize);
        agents[i].other_brain.for_each_weight(randomize);
        agents[i].body.x = RADIUS + i * RADIUS * 4;
        agents[i].body.y = RADIUS + i * RADIUS * 4;
        agents[i].body.vel_x = 0;
        agents[i].body.vel_y = 0;
        agents[i].body.ang = 0;
        agents[i].body.vel_ang = 0;
    }
    for (;;) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                break;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < N_AGENTS; ++i) {
            draw_body(renderer, agents[i].body, RADIUS, 0.2, 512, 360);
        }
        for (int i = 0; i < N_AGENTS; ++i) {
            agents[i].body.tick();
#if 1
            agents[i].body.vel_x *= 0.95;
            agents[i].body.vel_y *= 0.95;
            agents[i].body.vel_ang *= 0.95;
#endif
        }
        for (int i = 0; i < N_AGENTS; ++i) {
            for (int j = 0; j < i; ++j) {
                agents[i].consider_other(agents[j]);
            }
            for (int j = i + 1; j < N_AGENTS; ++j) {
                agents[i].consider_other(agents[j]);
            }
        }
        for (int i = 0; i < N_AGENTS; ++i) {
            agents[i].act(1, 0.05);
        }
        for (int i = 0; i < N_AGENTS; ++i) {
            for (int j = i + 1; j < N_AGENTS; ++j) {
                agents[i].body.collide(agents[j].body, RADIUS);
            }
        }
        SDL_RenderPresent(renderer);
    }
}

int main(void)
{
    int status = EXIT_FAILURE;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL initialization failed; %s\n", SDL_GetError());
        goto error_sdl_init;
    }
    window = SDL_CreateWindow("Bouncers", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 1024, 720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        fprintf(stderr, "SDL window creation failed; %s\n", SDL_GetError());
        goto error_create_window;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "SDL renderer creation failed; %s\n", SDL_GetError());
        goto error_create_surface;
    }
    simulate(renderer);
    status = EXIT_SUCCESS;
    SDL_DestroyRenderer(renderer);
error_create_surface:
    SDL_DestroyWindow(window);
error_create_window:
    SDL_Quit();
error_sdl_init:
    exit(status);
}
