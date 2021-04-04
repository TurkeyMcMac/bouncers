
#ifndef BOUNCERS_SCORE_H_
#define BOUNCERS_SCORE_H_

#include "Body.hpp"
#include "conf.hpp"
#include "math.hpp"
#include "scalar.hpp"
#include <cmath>

namespace bouncers {

namespace score {

    bool after_collision(int time, Body bodies[2], scalar scores[2])
    {
        (void)time;
        (void)bodies;
        (void)scores;
        return false;
    }

    bool after_tick(int time, Body bodies[2], scalar scores[2])
    {
        (void)time;
        for (int i = 0; i < 2; ++i) {
            if (std::hypot(bodies[i].x, bodies[i].y)
                > conf::START_DIST + conf::RADIUS) {
                scores[i] = -1;
                return true;
            }
        }
        return false;
    }

    void before_end(int time, Body bodies[2], scalar scores[2])
    {
        if (time >= conf::MAX_DURATION) {
            int winner = std::hypot(bodies[0].x, bodies[0].y)
                    < std::hypot(bodies[1].x, bodies[1].y)
                ? 0
                : 1;
            scores[winner] = 1;
        }
    }

}

} /* namespace bouncers */

#endif /* BOUNCERS_SCORE_H_ */
