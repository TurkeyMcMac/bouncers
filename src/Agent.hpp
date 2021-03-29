#ifndef BOUNCERS_AGENT_H_
#define BOUNCERS_AGENT_H_

#include "Body.hpp"
#include "Network.hpp"
#include "scalar.hpp"

namespace bouncers {

struct Agent {
    static constexpr int MEMORY_SIZE = 5;
    static constexpr int SELF_BRAIN_IN = MEMORY_SIZE + 2 + 2 + 1;
    static constexpr int SELF_BRAIN_MID = SELF_BRAIN_IN;
    static constexpr int SELF_BRAIN_OUT = MEMORY_SIZE + 2;
    static constexpr int OTHER_BRAIN_IN = MEMORY_SIZE + 2 + 2 + 1 + 1;
    static constexpr int OTHER_BRAIN_MID = OTHER_BRAIN_IN;
    static constexpr int OTHER_BRAIN_OUT = MEMORY_SIZE;

    scalar memory[MEMORY_SIZE];
    Network<SELF_BRAIN_IN, SELF_BRAIN_MID, SELF_BRAIN_OUT> self_brain;
    Network<OTHER_BRAIN_IN, OTHER_BRAIN_MID, OTHER_BRAIN_OUT> other_brain;
    Body body;

    void consider_other(const Agent& other);

    void act(scalar forward_speed, scalar turn_speed);
};

} /* namespace bouncers */

#endif /* BOUNCERS_AGENT_H_ */
