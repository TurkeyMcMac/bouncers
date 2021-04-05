#include "drawing.hpp"
#include "conf.hpp"
#include "math.hpp"
#include <algorithm>
#include <cmath>

namespace bouncers {

void draw_circle(SDL_Renderer* renderer, scalar x, scalar y, scalar radius)
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

void draw_digit(SDL_Renderer* renderer, int digit, scalar x, scalar y,
    scalar width, scalar height)
{
    if (width >= 1 && height >= 1 && digit >= 0 && digit <= 9) {
        static const struct segments {
            bool top, hi_left, hi_right, mid, lo_left, lo_right, bot;
        } segments[10] = {
            { 1, 1, 1, 0, 1, 1, 1 }, // 0
            { 0, 0, 1, 0, 0, 1, 0 }, // 1
            { 1, 0, 1, 1, 1, 0, 1 }, // 2
            { 1, 0, 1, 1, 0, 1, 1 }, // 3
            { 0, 1, 1, 1, 0, 1, 0 }, // 4
            { 1, 1, 0, 1, 0, 1, 1 }, // 5
            { 1, 1, 0, 1, 1, 1, 1 }, // 6
            { 1, 0, 1, 0, 0, 1, 0 }, // 7
            { 1, 1, 1, 1, 1, 1, 1 }, // 8
            { 1, 1, 1, 1, 0, 1, 0 }, // 9
        };
        int left_x = std::round(x);
        int right_x = std::round(x + width - 1);
        int top_y = std::round(y);
        int mid_y = std::round(y + (height - 1) / 2);
        int bot_y = std::round(y + height - 1);
        if (segments[digit].top)
            SDL_RenderDrawLine(renderer, left_x, top_y, right_x, top_y);
        if (segments[digit].hi_left)
            SDL_RenderDrawLine(renderer, left_x, top_y, left_x, mid_y);
        if (segments[digit].hi_right)
            SDL_RenderDrawLine(renderer, right_x, top_y, right_x, mid_y);
        if (segments[digit].mid)
            SDL_RenderDrawLine(renderer, left_x, mid_y, right_x, mid_y);
        if (segments[digit].lo_left)
            SDL_RenderDrawLine(renderer, left_x, mid_y, left_x, bot_y);
        if (segments[digit].lo_right)
            SDL_RenderDrawLine(renderer, right_x, mid_y, right_x, bot_y);
        if (segments[digit].bot)
            SDL_RenderDrawLine(renderer, left_x, bot_y, right_x, bot_y);
    }
}

void draw_number(SDL_Renderer* renderer, long n, scalar x, scalar y,
    scalar width, scalar height)
{
    int digits = std::ceil(std::log10((double)std::max(n, 1L) + 1));
    int x_div = digits * 2 + 1;
    scalar max_div_x = (scalar)width / x_div;
    scalar max_div_y = (scalar)height / 4;
    scalar div;
    if (max_div_x < max_div_y) {
        div = max_div_x;
        y += (max_div_y - max_div_x) * 2;
    } else {
        div = max_div_y;
        x += (width - max_div_y * x_div) / 2;
    }
    do {
        x_div -= 2;
        draw_digit(renderer, n % 10, x + x_div * div, y + div, div, div * 2);
        n /= 10;
    } while (n > 0);
}

} /* namespace bouncers */
