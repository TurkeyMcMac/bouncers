#ifndef BOUNCERS_SCORE_H_
#define BOUNCERS_SCORE_H_

#include "Body.hpp"
#include "scalar.hpp"

namespace bouncers {

namespace score {

    bool after_collision(int time, Body bodies[2], scalar scores[2]);

    bool after_tick(int time, Body bodies[2], scalar scores[2]);

    void before_end(int time, Body bodies[2], scalar scores[2]);

}

} /* namespace bouncers */

#endif /* BOUNCERS_SCORE_H_ */
