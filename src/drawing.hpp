#ifndef BOUNCERS_DRAWING_H_
#define BOUNCERS_DRAWING_H_

#include "scalar.hpp"
#include <SDL2/SDL_render.h>

namespace bouncers {

// Misc. drawing routines. The scalar parameters will be rounded for SDL.

// Draws an approximation of a circle with the renderer.
void draw_circle(SDL_Renderer* renderer, scalar x, scalar y, scalar radius);

// Draws a digit from 0-9 with the renderer in the available space.
void draw_digit(SDL_Renderer* renderer, int digit, scalar x, scalar y,
    scalar width, scalar height);

// Draws a NON-NEGATIVE number with appropriate size and padding centered in the
// available space.
void draw_number(SDL_Renderer* renderer, long n, scalar x, scalar y,
    scalar width, scalar height);

} /* namespace bouncers */

#endif /* BOUNCERS_DRAWING_H_ */
