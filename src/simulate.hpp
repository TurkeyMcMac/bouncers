#ifndef BOUNCERS_SIMULATE_H_
#define BOUNCERS_SIMULATE_H_

#include <SDL2/SDL_render.h>

namespace bouncers {

// Runs the main simulation with the renderer and the given RNG seed.
void simulate(SDL_Renderer* renderer, unsigned long seed);

} /* namespace bouncers */

#endif /* BOUNCERS_SIMULATE_H_ */
