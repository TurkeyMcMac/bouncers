#include "Agent.hpp"
#include "align.hpp"
#include "math.hpp"
#include "scalar.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <random>
#include <thread>

using namespace bouncers;

struct alignas(CACHE_LINE_SIZE) AlignedAgent {
    Agent a;
};

static const scalar RADIUS = 120;
static const scalar START_DIST = 500;
static const int N_AGENTS = 50;
static const int ROUND_DURATION = 600;
static const int MAX_THREADS = N_AGENTS / 2;

static void draw_circle(
    SDL_Renderer* renderer, scalar x, scalar y, scalar radius)
{
    static const int N_POINTS = 32;
    SDL_Point points[N_POINTS];
    for (int i = 0; i < N_POINTS - 1; ++i) {
        scalar theta = TAU / (N_POINTS - 1) * i;
        points[i].x = std::round(x + std::cos(theta) * radius);
        points[i].y = std::round(y + std::sin(theta) * radius);
    }
    points[N_POINTS - 1] = points[0];
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

static void make_random_agents(
    AlignedAgent agents[N_AGENTS], std::minstd_rand& rand)
{
    std::uniform_real_distribution<scalar> real_dis(-0.1, 0.1);
    auto randomize
        = [&rand, &real_dis](scalar& w) mutable { w = real_dis(rand); };
    for (int i = 0; i < N_AGENTS; ++i) {
        agents[i].a.brain.for_each_weight(randomize);
    }
}

static bool breed_winner(SDL_Renderer* renderer, AlignedAgent agents[2])
{
    int winner = 0;
    Agent my_agents[2] = { agents[0].a, agents[1].a };
    Body bodies[2];
    bodies[0].x = -START_DIST;
    bodies[0].y = 0;
    bodies[0].vel_x = 0;
    bodies[0].vel_y = 0;
    bodies[0].ang = 0;
    bodies[1].x = +START_DIST;
    bodies[1].y = 0;
    bodies[1].vel_x = 0;
    bodies[1].vel_y = 0;
    bodies[1].ang = PI;
    for (int t = 0; t < ROUND_DURATION; ++t) {
        Uint32 ticks = 0;
        if (renderer) {
            ticks = SDL_GetTicks();
            SDL_Event event;
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    return false;
            }
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int i = 0; i < 2; ++i) {
                draw_body(renderer, bodies[i], RADIUS, 0.2, 512, 360);
            }
            draw_circle(renderer, 512, 360, 4);
            draw_circle(renderer, 512, 360, START_DIST * 0.2);
            SDL_RenderPresent(renderer);
        }
        for (int i = 0; i < 2; ++i) {
            bodies[i].tick();
            bodies[i].vel_x *= 0.98;
            bodies[i].vel_y *= 0.98;
        }
        Body old_body_0 = bodies[0];
        my_agents[0].act(bodies[0], bodies[1], 1, 0.1);
        my_agents[1].act(bodies[1], old_body_0, 1, 0.1);
        bodies[0].collide(bodies[1], RADIUS);
        for (int i = 0; i < 2; ++i) {
            if (std::hypot(bodies[i].x, bodies[i].y) > START_DIST + RADIUS) {
                winner = 1 - i;
                goto end;
            }
        }
        if (renderer) {
            Uint32 new_ticks = SDL_GetTicks();
            if (new_ticks - ticks < 20)
                SDL_Delay(20 - (new_ticks - ticks));
        }
    }
    winner = std::hypot(bodies[0].x, bodies[0].y)
            < std::hypot(bodies[1].x, bodies[1].y)
        ? 0
        : 1;
end:
    agents[1 - winner] = agents[winner];
    return true;
}

static void mutate_agent(Agent& agent, std::minstd_rand& rand)
{
    std::uniform_real_distribution<scalar> real_dis(-1, 1);
    auto mutate = [&rand, &real_dis](scalar& w) mutable {
        if (real_dis(rand) > 0.97)
            w += real_dis(rand);
    };
    agent.brain.for_each_weight(mutate);
}

static void simulate(SDL_Renderer* renderer, unsigned seed)
{
    AlignedAgent agents[N_AGENTS];
    std::minstd_rand rand(seed);
    make_random_agents(agents, rand);
    int n_threads = std::min(SDL_GetCPUCount(), MAX_THREADS);
    std::thread* threads
        = (std::thread*)std::malloc(n_threads * sizeof(*threads));
    if (!threads)
        throw std::bad_alloc();
    bool keep_going = true;
    for (long t = 0; keep_going; ++t) {
        if (t % 100 == 0)
            std::printf("Round %ld\n", t);
        AlignedAgent visualized_agents[2] = { agents[0], agents[1] };
        alignas(CACHE_LINE_SIZE) std::atomic<int> place(0);
        for (int i = 0; i < n_threads; ++i) {
            new (&threads[i]) std::thread([&agents, &place]() {
                int j;
                while ((j = place.fetch_add(2, std::memory_order_relaxed)) + 1
                    < N_AGENTS) {
                    breed_winner(NULL, agents + j);
                }
            });
        }
        SDL_Event event;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                keep_going = false;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_SPACE)
                    keep_going = breed_winner(renderer, visualized_agents);
                break;
            }
        }
        for (int i = 0; i < n_threads; ++i) {
            threads[i].join();
            threads[i].~thread();
        }
        for (int i = 0; i < N_AGENTS; ++i) {
            mutate_agent(agents[i].a, rand);
        }
        std::shuffle(agents, agents + N_AGENTS, rand);
    }
    std::free(threads);
}

int main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL initialization failed; %s\n", SDL_GetError());
        goto error_sdl_init;
    }
    window = SDL_CreateWindow("Bouncers", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, 1024, 720,
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        std::fprintf(
            stderr, "SDL window creation failed; %s\n", SDL_GetError());
        goto error_create_window;
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        std::fprintf(
            stderr, "SDL renderer creation failed; %s\n", SDL_GetError());
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
