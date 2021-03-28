#include "Body.hpp"
#include "scalar.hpp"
#include <SDL2/SDL.h>
#include <cmath>
#include <cstdlib>

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
        points[i].x = x + offsets[i].x * radius;
        points[i].y = y + offsets[i].y * radius;
    }
    SDL_RenderDrawLines(renderer, points, N_POINTS);
}

static void draw_body(SDL_Renderer* renderer, const Body& body, scalar radius)
{
    draw_circle(renderer, body.x, body.y, radius);
    scalar head_x = body.x + std::cos(body.ang) * radius;
    scalar head_y = body.y + std::sin(body.ang) * radius;
    SDL_RenderDrawLine(renderer, body.x, body.y, head_x, head_y);
}

static void simulate(SDL_Renderer* renderer)
{
    static const scalar RADIUS = 40;
    static const int N_BODIES = 4;
    Body bodies[N_BODIES];
    for (int i = 0; i < N_BODIES; ++i) {
        bodies[i].x = RADIUS + i * RADIUS * 4;
        bodies[i].y = RADIUS + i * RADIUS * 4;
        bodies[i].vel_x = 0;
        bodies[i].vel_y = 0;
        bodies[i].ang = 0;
        bodies[i].vel_ang = -0.2;
    }
    bodies[0].vel_x = 2;
    bodies[0].vel_y = 2.2;
    bodies[0].vel_ang = 0.2;
#if 0
    bodies[1].vel_x = 1.6;
    bodies[1].vel_y = -1.6;
    bodies[1].vel_ang = 0.1;
#endif
    int key = 0;
    bool move_forward = false, turn_left = false, turn_right = false;
    for (;;) {
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                break;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                case 'w':
                    move_forward = event.type == SDL_KEYDOWN;
                    break;
                case 'a':
                    turn_left = event.type == SDL_KEYDOWN;
                    break;
                case 'd':
                    turn_right = event.type == SDL_KEYDOWN;
                    break;
                }
            }
        }
        if (move_forward) {
            bodies[0].vel_x += std::cos(bodies[0].ang) * 0.09;
            bodies[0].vel_y += std::sin(bodies[0].ang) * 0.09;
        }
        if (turn_left)
            bodies[0].vel_ang -= 0.02;
        if (turn_right)
            bodies[0].vel_ang += 0.02;
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        for (int i = 0; i < N_BODIES; ++i) {
            draw_body(renderer, bodies[i], RADIUS);
        }
        scalar lm_x = 0, lm_y = 0, am = 0, e = 0;
        for (int i = 0; i < N_BODIES; ++i) {
#if 1
            bodies[i].vel_x *= 0.99;
            bodies[i].vel_y *= 0.99;
            bodies[i].vel_ang *= 0.99;
#endif
            lm_x += bodies[i].vel_x;
            lm_y += bodies[i].vel_y;
            am += bodies[i].vel_ang;
            e += bodies[i].vel_x * bodies[i].vel_x
                + bodies[i].vel_y * bodies[i].vel_y
                + RADIUS * bodies[i].vel_ang * RADIUS * bodies[i].vel_ang;
            bodies[i].tick();
        }
        printf("lm = (%f, %f), am = %f, e = %f\n", lm_x, lm_y, am, e);
        for (int i = 0; i < N_BODIES; ++i) {
            for (int j = i + 1; j < N_BODIES; ++j) {
                bodies[i].collide(bodies[j], RADIUS);
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
