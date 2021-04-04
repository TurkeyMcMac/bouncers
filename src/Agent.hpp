#ifndef BOUNCERS_AGENT_H_
#define BOUNCERS_AGENT_H_

#include "Body.hpp"
#include "Network.hpp"
#include "conf.hpp"
#include "scalar.hpp"

namespace bouncers {

struct Agent {
    static constexpr int BRAIN_IN = 2 + 2 + 2 + 2 + 1 + 1;
    static constexpr int BRAIN_MID = conf::N_MIDDLE_NEURONS;
    static constexpr int BRAIN_OUT = 2;

    Network<BRAIN_IN, BRAIN_MID, BRAIN_OUT> brain;

    void act(Body& self, Body other, scalar straight_acc, scalar turn_speed,
        scalar time);
};

} /* namespace bouncers */

#endif /* BOUNCERS_AGENT_H_ */
