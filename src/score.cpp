#include "Body.hpp"
#include "conf.hpp"
#include "math.hpp"
#include "scalar.hpp"
#include <cmath>

// These functions let you control how agents are scored. They take the time as
// a number of ticks that have so far passed, the two bodies, and the two
// scores. If they return true, the round ends.

namespace bouncers {

namespace score {

    // Runs after the agents collide. time < conf::MAX_DURATION.
    bool after_collision(int time, Body bodies[2], scalar scores[2])
    {
        // By default, nothing is done here.
        (void)time;
        (void)bodies;
        (void)scores;
        return false;
    }

    // Runs after every tick. time < conf::MAX_DURATION.
    bool after_tick(int time, Body bodies[2], scalar scores[2])
    {
        // By default, agents get more points per tick the closer they are to
        // the edge, but if they go past the edge, they lose.
        (void)time;
        for (int i = 0; i < 2; ++i) {
            scalar dist = std::hypot(bodies[i].x, bodies[i].y);
            if (dist > conf::START_DIST + conf::RADIUS) {
                // This agent loses.
                scores[i] = -1;
                return true;
            }
            // Score this tick scales with distance.
            scores[i] += dist;
        }
        return false;
    }

    // Runs right before the winner is decided, including when one of the two
    // functions above ends the round by returning true.
    // time <= conf::MAX_DURATION.
    void before_end(int time, Body bodies[2], scalar scores[2])
    {
        // By default, nothing is done here.
        (void)time;
        (void)bodies;
        (void)scores;
    }

}

} /* namespace bouncers */
