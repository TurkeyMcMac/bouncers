#ifndef BOUNCERS_AGENT_H_
#define BOUNCERS_AGENT_H_

#include "Body.hpp"
#include "Network.hpp"
#include "conf.hpp"
#include "scalar.hpp"

namespace bouncers {

// An agent is a being in the simulation. This struct does not include the body
// as agents are stored without bodies between rounds. Thus, this struct is just
// a neural network with a certain size that takes care of marshalling data from
// the environment and applying the network's output to control a Body.
struct Agent {
    static constexpr int BRAIN_IN = 2 + 2 + 2 + 2 + 1 + 1;
    static constexpr int BRAIN_MID = conf::N_MIDDLE_NEURONS;
    static constexpr int BRAIN_OUT = 2;

    Network<BRAIN_IN, BRAIN_MID, BRAIN_OUT> brain;

    // Takes action (modifying the Body self) based on the position of the other
    // and the time (an quantity proportional to the time since the beginning.)
    // straight_acc is the maximum magnitude of forward/backward acceleration
    // and turn_speed is the maximum turn speed in radians. The Agent struct
    // itself is not modified by this method.
    void act(Body& self, Body other, scalar time, scalar straight_acc,
        scalar turn_speed);
};

} /* namespace bouncers */

#endif /* BOUNCERS_AGENT_H_ */
