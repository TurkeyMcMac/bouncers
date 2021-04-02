#include "Agent.hpp"
#include "constants.hpp"
#include "scalar.hpp"
#include <SDL2/SDL.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <random>
#include <thread>

using namespace bouncers;

static const scalar RADIUS = 120;
static const scalar START_DIST = 500;
static const int N_AGENTS = 50;
static const int ROUND_DURATION = 600;
static const int MAX_THREADS = 128;

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

static void make_random_agents(Agent agents[N_AGENTS], std::minstd_rand& rand)
{
    std::uniform_real_distribution<scalar> real_dis(-0.1, 0.1);
    auto randomize
        = [&rand, &real_dis](scalar& w) mutable { w = real_dis(rand); };
    for (int i = 0; i < N_AGENTS; ++i) {
        agents[i].brain.for_each_weight(randomize);
        agents[i].body.x = 0;
        agents[i].body.y = 0;
        agents[i].body.vel_x = 0;
        agents[i].body.vel_y = 0;
        agents[i].body.ang = 0;
    }
}

static bool breed_winner(SDL_Renderer* renderer, Agent agents[2])
{
    int winner = 0;
    Agent my_agents[2] = { agents[0], agents[1] };
    my_agents[0].body.x = -START_DIST;
    my_agents[0].body.y = 0;
    my_agents[0].body.ang = 0;
    my_agents[1].body.x = +START_DIST;
    my_agents[1].body.y = 0;
    my_agents[1].body.ang = PI;
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
                draw_body(renderer, my_agents[i].body, RADIUS, 0.2, 512, 360);
            }
            draw_circle(renderer, 512, 360, 4);
            draw_circle(renderer, 512, 360, START_DIST * 0.2);
            SDL_RenderPresent(renderer);
        }
        for (int i = 0; i < 2; ++i) {
            my_agents[i].body.tick();
            my_agents[i].body.vel_x *= 0.98;
            my_agents[i].body.vel_y *= 0.98;
        }
        for (int i = 0; i < 2; ++i) {
            my_agents[i].act(my_agents[1 - i].body, 1, 0.1);
        }
        my_agents[0].body.collide(my_agents[1].body, RADIUS);
        for (int i = 0; i < 2; ++i) {
            if (std::hypot(my_agents[i].body.x, my_agents[i].body.y)
                > START_DIST + RADIUS) {
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
    winner = std::hypot(my_agents[0].body.x, my_agents[0].body.y)
            < std::hypot(my_agents[1].body.x, my_agents[1].body.y)
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
    Agent agents[N_AGENTS];
    std::minstd_rand rand(seed);
    make_random_agents(agents, rand);
    bool keep_going = true;
    for (long t = 0; keep_going; ++t) {
        if (t % 10 == 0)
            printf("Round %ld\n", t);
        Agent visualized_agents[2] = { agents[0], agents[1] };
        int n_threads = std::min(SDL_GetCPUCount(), MAX_THREADS);
        std::thread threads[MAX_THREADS];
        std::atomic<int> place(0);
        for (int i = 0; i < n_threads; ++i) {
            new (&threads[i]) std::thread(
                [&agents](std::atomic<int>* place) {
                    int j;
                    while ((j = place->fetch_add(2)) + 1 < N_AGENTS) {
                        breed_winner(NULL, agents + j);
                    }
                },
                &place);
        }
        if (t % 500 == 0) {
            keep_going = breed_winner(renderer, visualized_agents);
        } else {
            SDL_Event event;
            if (SDL_PollEvent(&event))
                keep_going = event.type != SDL_QUIT;
        }
        for (int i = 0; i < n_threads; ++i) {
            threads[i].join();
        }
        for (int i = 0; i < N_AGENTS; ++i) {
            mutate_agent(agents[i], rand);
        }
        std::shuffle(agents, agents + N_AGENTS, rand);
    }
}

int main(int argc, char* argv[])
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
    simulate(renderer, argc >= 2 ? (unsigned)atoi(argv[1]) : 1337U);
    status = EXIT_SUCCESS;
    SDL_DestroyRenderer(renderer);
error_create_surface:
    SDL_DestroyWindow(window);
error_create_window:
    SDL_Quit();
error_sdl_init:
    exit(status);
}
