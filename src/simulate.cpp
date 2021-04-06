#include "simulate.hpp"
#include "Agent.hpp"
#include "align.hpp"
#include "conf.hpp"
#include "drawing.hpp"
#include "math.hpp"
#include "scalar.hpp"
#include "score.hpp"
#include <SDL2/SDL_cpuinfo.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <random>
#include <thread>

namespace bouncers {

// The AlignedAgent buffer is aligned to 2 * alignof(AlignedAgent), meaning it
// is aligned to THREAD_SEP_ALIGN. An AlignedAgent can have an alignment less
// than THREAD_SEP_ALIGN as long as an agent pair has an alignment of
// THREAD_SEP_ALIGN to prevent false sharing.
struct alignas(
    std::max(THREAD_SEP_ALIGN / 2, (int)alignof(Agent))) AlignedAgent {
    Agent a;
};

static void draw_body(SDL_Renderer* renderer, const Body& body, scalar radius,
    bool winning, scalar screen_scale, int screen_origin_x, int screen_origin_y)
{
    scalar screen_x = body.x * screen_scale + screen_origin_x;
    scalar screen_y = body.y * screen_scale + screen_origin_y;
    scalar screen_radius = radius * screen_scale;
    draw_circle(renderer, screen_x, screen_y, screen_radius);
    if (winning)
        draw_circle(renderer, screen_x, screen_y, screen_radius / 2);
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

// Runs a round. If a non-null renderer is given, the round will be animated.
// false is returned when the user quits during the animation. When true is
// returned, the winning agent is copied into the place that held the loser.
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
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT)
                    return false;
            }
            SDL_Rect viewport;
            SDL_RenderGetViewport(renderer, &viewport);
            int screen_center_x = viewport.x + viewport.w / 2;
            int screen_center_y = viewport.y + viewport.h / 2;
            // The scale is the conversion from simulation coordinates to
            // viewport coordinates.
            scalar scale
                = std::min(viewport.w, viewport.h) / (conf::START_DIST * 4);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            draw_body(renderer, bodies[0], conf::RADIUS, scores[0] > scores[1],
                scale, screen_center_x, screen_center_y);
            draw_body(renderer, bodies[1], conf::RADIUS, scores[1] > scores[0],
                scale, screen_center_x, screen_center_y);
            // Draw the ring and a dot at its center.
            draw_circle(renderer, screen_center_x, screen_center_y, 1);
            draw_circle(renderer, screen_center_x, screen_center_y,
                conf::START_DIST * scale);
            // number_dim is a width and height in screen coordinates that can
            // hold the time display without touching the ring drawn above.
            scalar number_dim = conf::START_DIST * scale;
            draw_number(
                renderer, conf::MAX_DURATION - t, 0, 0, number_dim, number_dim);
            SDL_RenderPresent(renderer);
        }
        bodies[0].tick(conf::DRAG);
        bodies[1].tick(conf::DRAG);
        Body old_body_0 = bodies[0];
        my_agents[0].act(
            bodies[0], bodies[1], t, conf::STRAIGHT_ACC, conf::TURN_SPEED);
        my_agents[1].act(
            bodies[1], old_body_0, t, conf::STRAIGHT_ACC, conf::TURN_SPEED);
        if (bodies[0].collide(bodies[1], conf::RADIUS)
            && score::after_collision(t, bodies, scores))
            break;
        if (score::after_tick(t, bodies, scores))
            break;
        if (renderer) {
            // Wait for the next simulation tick.
            Uint32 new_ticks = SDL_GetTicks();
            if (new_ticks - ticks < (Uint32)conf::FRAME_TIME)
                SDL_Delay(conf::FRAME_TIME - (new_ticks - ticks));
        }
    }
    score::before_end(t, bodies, scores);
    // Breed the winner.
    if (scores[0] > scores[1]) {
        agents[1] = agents[0];
    } else {
        agents[0] = agents[1];
    }
    return true;
}

static void mutate_agent(Agent& agent, std::minstd_rand& rand)
{
    // The chance that dis(rand) > MUTATION_THRESHOLD is conf::MUTATION_CHANCE.
    static const scalar MUTATION_THRESHOLD
        = conf::MUTATION * (1 - conf::MUTATION_CHANCE * 2);
    std::uniform_real_distribution<scalar> dis(-conf::MUTATION, conf::MUTATION);
    auto mutate = [&rand, &dis](scalar& w) mutable {
        if (dis(rand) > MUTATION_THRESHOLD)
            w += dis(rand);
    };
    agent.brain.for_each_weight(mutate);
}

void simulate(SDL_Renderer* renderer, unsigned long seed)
{
    AlignedAgent* agents_buf;
    AlignedAgent* const agents
        = (AlignedAgent*)aligned_alloc(alignof(AlignedAgent) * 2,
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
    bool running = true;
    bool keep_going = true;
    for (long t = 0; keep_going; t += running) {
        // The simulation keeps running/not running unless paused/unpaused.
        bool running_next_time = running;
        // These agents are used when animating a round.
        AlignedAgent visualized_agents[2] = { agents[0], agents[1] };
        if (running) {
            // The threads coordinate which agents to simulate next using this
            // atomic counter.
            alignas(THREAD_SEP_ALIGN) std::atomic<int> place(0);
            for (int i = 0; i < n_threads; ++i) {
                new (&threads[i]) std::thread([agents, &place]() {
                    int j;
                    // Keep taking pairs off the queue until none are left.
                    while (
                        (j = place.fetch_add(2, std::memory_order_relaxed)) + 1
                        < conf::N_AGENTS) {
                        breed_winner(nullptr, agents + j);
                    }
                });
            }
        }
        long since_count = t % (long)conf::GEN_COUNT_INTERVAL;
        bool redraw = since_count == 0 && running;
        bool show_breeding = false;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                keep_going = false;
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym) {
                case SDLK_RETURN:
                    // Show an animated round.
                    show_breeding = true;
                    break;
                case SDLK_SPACE:
                    // (Un)pause.
                    running_next_time = !running;
                    break;
                }
                break;
            case SDL_WINDOWEVENT:
                redraw = redraw || event.window.event == SDL_WINDOWEVENT_SHOWN
                    || event.window.event == SDL_WINDOWEVENT_RESTORED
                    || event.window.event == SDL_WINDOWEVENT_EXPOSED
                    || event.window.event == SDL_WINDOWEVENT_RESIZED;
                break;
            }
        }
        if (show_breeding) {
            keep_going = breed_winner(renderer, visualized_agents);
            redraw = true;
        }
        if (redraw) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Rect viewport;
            SDL_RenderGetViewport(renderer, &viewport);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            // since_count is subtracted from the displayed time to round it
            // down to the nearest multiple of conf::GEN_COUNT_INTERVAL.
            draw_number(
                renderer, t - since_count, 0, 0, viewport.w, viewport.h);
            SDL_RenderPresent(renderer);
        }
        // Prevent useless CPU spinning.
        if (!running)
            SDL_Delay(conf::FRAME_TIME);
        if (running) {
            for (int i = 0; i < n_threads; ++i) {
                threads[i].join();
                threads[i].~thread();
            }
            for (int i = 0; i < conf::N_AGENTS; ++i) {
                mutate_agent(agents[i].a, rand);
            }
            // The agents are shuffled to make new matchups next generation.
            std::shuffle(agents, agents + conf::N_AGENTS, rand);
        }
        running = running_next_time;
    }
    std::free(threads);
    std::free(agents_buf);
}

} /* namespace bouncers */
