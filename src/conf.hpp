#ifndef BOUNCERS_CONF_H_
#define BOUNCERS_CONF_H_

#include "scalar.hpp"

namespace bouncers {

namespace conf {
    // The portion of agent velocity that evaporates per tick.
    static constexpr scalar DRAG = 0.02;
    // The minimum number of milliseconds between frames of an animated round.
    static constexpr int FRAME_TIME = 30;
    // The number of generations between updates to the on-screen count.
    static constexpr int GEN_COUNT_INTERVAL = 100;
    // The maximum value of an absolute weight in one of the initial randomly
    // generated agents.
    static constexpr scalar INITIAL_VARIATION = 0.1;
    // The maximum number of ticks in a round.
    static constexpr int MAX_DURATION = 600;
    // The maximum number of threads to use while simulating rounds. The program
    // will also never use more threads than there are available logical cores.
    static constexpr int MAX_THREADS = 25;
    // The maximum absolute change in a weight when it is randomly mutated.
    static constexpr scalar MUTATION = 1;
    // The chance that any given weight will be mutated for the next generation.
    static constexpr scalar MUTATION_CHANCE = 0.01;
    // The number of agents in each generation. If this is odd, then a random
    // agent will be the odd one out each generation, and will get to have one
    // child without a contest.
    static constexpr int N_AGENTS = 50;
    // The number of points on the regular polygon used to approximate a circle
    // on the screen.
    static constexpr int N_CIRCLE_POINTS = 32;
    // The number of neurons in the middle layer of an agent's neural network.
    static constexpr int N_MIDDLE_NEURONS = 16;
    // The radius of the body of an agent.
    static constexpr scalar RADIUS = 120;
    // The distance of each agents starting position from the origin at the
    // beginning of every round.
    static constexpr scalar START_DIST = 500;
    // The maximum forward/backward acceleration an agent can apply per tick.
    static constexpr scalar STRAIGHT_ACC = 1;
    // The maximum turn speed in radians.
    static constexpr scalar TURN_SPEED = 0.1;
    // The initial width of the window in SDL screen coordinates.
    static constexpr int WINDOW_WIDTH = 200;
    // The initial height of the window in SDL screen coordinates.
    static constexpr int WINDOW_HEIGHT = 200;
}

} /* namespace bouncers */

#endif /* BOUNCERS_CONF_H_ */
