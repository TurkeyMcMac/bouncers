#ifndef BOUNCERS_CONF_H_
#define BOUNCERS_CONF_H_

#include "scalar.hpp"

namespace bouncers {

namespace conf {
    static constexpr scalar DRAG = 0.02;
    static constexpr int FRAME_TIME = 20;
    static constexpr int GEN_COUNT_INTERVAL = 100;
    static constexpr scalar INITIAL_VARIATION = 0.1;
    static constexpr int MAX_DURATION = 600;
    static constexpr int MAX_THREADS = 25;
    static constexpr scalar MUTATION = 1;
    static constexpr scalar MUTATION_CHANCE = 0.01;
    static constexpr int N_AGENTS = 50;
    static constexpr int N_CIRCLE_POINTS = 32;
    static constexpr int N_MIDDLE_NEURONS = 16;
    static constexpr scalar RADIUS = 120;
    static constexpr scalar START_DIST = 500;
    static constexpr scalar STRAIGHT_ACC = 1;
    static constexpr scalar TURN_SPEED = 0.1;
    static constexpr int WINDOW_WIDTH = 200;
    static constexpr int WINDOW_HEIGHT = 200;
}

} /* namespace bouncers */

#endif /* BOUNCERS_CONF_H_ */
