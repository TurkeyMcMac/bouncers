#include "Agent.hpp"
#include "align.hpp"
#include "conf.hpp"
#include "math.hpp"
#include "scalar.hpp"
#include "score.hpp"
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

static void draw_circle(
    SDL_Renderer* renderer, scalar x, scalar y, scalar radius)
{
    SDL_Point points[conf::N_CIRCLE_POINTS];
    for (int i = 0; i < conf::N_CIRCLE_POINTS - 1; ++i) {
        scalar theta = TAU / (conf::N_CIRCLE_POINTS - 1) * i;
        points[i].x = std::round(x + std::cos(theta) * radius);
        points[i].y = std::round(y + std::sin(theta) * radius);
    }
    points[conf::N_CIRCLE_POINTS - 1] = points[0];
    SDL_RenderDrawLines(renderer, points, conf::N_CIRCLE_POINTS);
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
    AlignedAgent agents[conf::N_AGENTS], std::minstd_rand& rand)
{
    std::uniform_real_distribution<scalar> dis(
        -conf::INITIAL_VARIATION, conf::INITIAL_VARIATION);
    auto randomize = [&rand, &dis](scalar& w) mutable { w = dis(rand); };
    for (int i = 0; i < conf::N_AGENTS; ++i) {
        agents[i].a.brain.for_each_weight(randomize);
    }
}

static bool breed_winner(SDL_Renderer* renderer, AlignedAgent agents[2])
{
    Agent my_agents[2] = { agents[0].a, agents[1].a };
    scalar scores[2] = { 0, 0 };
    Body bodies[2];
    bodies[0].x = -conf::START_DIST;
    bodies[0].y = 0;
    bodies[0].vel_x = 0;
    bodies[0].vel_y = 0;
    bodies[0].ang = 0;
    bodies[1].x = +conf::START_DIST;
    bodies[1].y = 0;
    bodies[1].vel_x = 0;
    bodies[1].vel_y = 0;
    bodies[1].ang = PI;
    int t;
    for (t = 0; t < conf::MAX_DURATION; ++t) {
        Uint32 ticks = 0;
        if (renderer) {
            ticks = SDL_GetTicks();
            SDL_Event event;
            if (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    return false;
            }
            SDL_Rect viewport;
            SDL_RenderGetViewport(renderer, &viewport);
            int screen_center_x = viewport.x + viewport.w / 2;
            int screen_center_y = viewport.y + viewport.h / 2;
            scalar scale
                = std::min(viewport.w, viewport.h) / (conf::START_DIST * 2);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            for (int i = 0; i < 2; ++i) {
                draw_body(renderer, bodies[i], conf::RADIUS, scale,
                    screen_center_x, screen_center_y);
            }
            draw_circle(renderer, screen_center_x, screen_center_y, 1);
            draw_circle(renderer, screen_center_x, screen_center_y,
                conf::START_DIST * scale);
            SDL_RenderPresent(renderer);
        }
        for (int i = 0; i < 2; ++i) {
            bodies[i].tick();
            bodies[i].vel_x *= 1 - conf::DRAG;
            bodies[i].vel_y *= 1 - conf::DRAG;
        }
        Body old_body_0 = bodies[0];
        my_agents[0].act(
            bodies[0], bodies[1], conf::STRAIGHT_ACC, conf::TURN_SPEED, t);
        my_agents[1].act(
            bodies[1], old_body_0, conf::STRAIGHT_ACC, conf::TURN_SPEED, t);
        if (bodies[0].collide(bodies[1], conf::RADIUS)
            && score::after_collision(t, bodies, scores))
            break;
        if (score::after_tick(t, bodies, scores))
            break;
        if (renderer) {
            Uint32 new_ticks = SDL_GetTicks();
            if (new_ticks - ticks < (Uint32)conf::FRAME_TIME)
                SDL_Delay(conf::FRAME_TIME - (new_ticks - ticks));
        }
    }
    score::before_end(t, bodies, scores);
    if (scores[0] > scores[1]) {
        agents[1] = agents[0];
    } else {
        agents[0] = agents[1];
    }
    return true;
}

static void mutate_agent(Agent& agent, std::minstd_rand& rand)
{
    static const scalar MUTATION_THRESHOLD
        = conf::MUTATION * (1 - conf::MUTATION_CHANCE * 2);
    std::uniform_real_distribution<scalar> dis(-conf::MUTATION, conf::MUTATION);
    auto mutate = [&rand, &dis](scalar& w) mutable {
        if (dis(rand) > MUTATION_THRESHOLD)
            w += dis(rand);
    };
    agent.brain.for_each_weight(mutate);
}

static void simulate(SDL_Renderer* renderer, unsigned seed)
{
    AlignedAgent* agents_buf;
    AlignedAgent* agents = (AlignedAgent*)aligned_alloc(alignof(AlignedAgent),
        conf::N_AGENTS * sizeof(AlignedAgent), (void*&)agents_buf);
    if (!agents)
        throw std::bad_alloc();
    int n_threads = std::min(SDL_GetCPUCount(), conf::MAX_THREADS);
    std::thread* threads
        = (std::thread*)std::malloc(n_threads * sizeof(*threads));
    if (!threads) {
        std::free(agents_buf);
        throw std::bad_alloc();
    }
    std::minstd_rand rand(seed);
    make_random_agents(agents, rand);
    bool keep_going = true;
    for (long t = 0; keep_going; ++t) {
        if (t % (long)conf::GEN_COUNT_INTERVAL == 0)
            std::printf("Generation %ld\n", t);
        AlignedAgent visualized_agents[2] = { agents[0], agents[1] };
        alignas(CACHE_LINE_SIZE) std::atomic<int> place(0);
        for (int i = 0; i < n_threads; ++i) {
            new (&threads[i]) std::thread([&agents, &place]() {
                int j;
                while ((j = place.fetch_add(2, std::memory_order_relaxed)) + 1
                    < conf::N_AGENTS) {
                    breed_winner(nullptr, agents + j);
                }
            });
        }
        SDL_Event event;
        bool clear = false;
        if (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                keep_going = false;
                break;
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_SPACE) {
                    keep_going = breed_winner(renderer, visualized_agents);
                    clear = true;
                }
                break;
            case SDL_WINDOWEVENT:
                clear = event.window.event == SDL_WINDOWEVENT_SHOWN
                    || event.window.event == SDL_WINDOWEVENT_RESTORED
                    || event.window.event == SDL_WINDOWEVENT_EXPOSED
                    || event.window.event == SDL_WINDOWEVENT_RESIZED;
                break;
            }
        }
        if (clear) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
        for (int i = 0; i < n_threads; ++i) {
            threads[i].join();
            threads[i].~thread();
        }
        for (int i = 0; i < conf::N_AGENTS; ++i) {
            mutate_agent(agents[i].a, rand);
        }
        std::shuffle(agents, agents + conf::N_AGENTS, rand);
    }
    std::free(threads);
    std::free(agents_buf);
}

int main(int argc, char* argv[])
{
    int status = EXIT_FAILURE;
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_VIDEO)) {
        std::fprintf(stderr, "SDL initialization failed; %s\n", SDL_GetError());
        goto error_sdl_init;
    }
    window = SDL_CreateWindow("Bouncers", SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED, conf::WINDOW_WIDTH, conf::WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
