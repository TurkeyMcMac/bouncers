#ifndef BOUNCERS_AGENT_H_
#define BOUNCERS_AGENT_H_

#include "Body.hpp"
#include "Network.hpp"
#include "constants.hpp"
#include "scalar.hpp"

namespace bouncers {

struct alignas(CACHE_LINE_SIZE) Agent {
    static constexpr int BRAIN_IN = 2 + 2 + 2 + 2 + 1;
    static constexpr int BRAIN_MID = BRAIN_IN + 7;
    static constexpr int BRAIN_OUT = 2;

    Network<BRAIN_IN, BRAIN_MID, BRAIN_OUT> brain;
    Body body;

    void act(Body other, scalar forward_speed, scalar turn_speed);
};

} /* namespace bouncers */

#endif /* BOUNCERS_AGENT_H_ */
