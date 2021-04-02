#ifndef BOUNCERS_BODY_H_
#define BOUNCERS_BODY_H_

#include "scalar.hpp"

namespace bouncers {

struct Body {
    scalar x, y;
    scalar vel_x, vel_y;
    scalar ang;

    void tick();

    bool collide(Body& other, scalar radius);
};

} /* namespace bouncers */

#endif /* BOUNCERS_BODY_H_ */
