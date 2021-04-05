#ifndef BOUNCERS_DRAWING_H_
#define BOUNCERS_DRAWING_H_

#include "scalar.hpp"
#include <SDL2/SDL_render.h>

namespace bouncers {

void draw_circle(SDL_Renderer* renderer, scalar x, scalar y, scalar radius);

void draw_digit(SDL_Renderer* renderer, int digit, scalar x, scalar y,
    scalar width, scalar height);

void draw_number(SDL_Renderer* renderer, long n, scalar x, scalar y,
    scalar width, scalar height);

} /* namespace bouncers */

#endif /* BOUNCERS_DRAWING_H_ */
