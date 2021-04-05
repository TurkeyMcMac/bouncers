#ifndef BOUNCERS_BODY_H_
#define BOUNCERS_BODY_H_

#include "scalar.hpp"

namespace bouncers {

// A Body represents the position of an agent.
struct Body {
    // The offset from the origin:
    scalar x, y;
    // The translational velocity:
    scalar vel_x, vel_y;
    // The angle, a small number of radians.
    scalar ang;

    // Simulates one tick of physics for the Body. ang is kept small, and the
    // given drag (in the range [0, 1]) is applied to the velocity. A drag of 0
    // has no effect on the velocity, and a drag of 1 stops the body in its
    // tracks.
    void tick(scalar drag);

    // Collide with the other body. For this collision, both bodies are circles
    // of the given radius. The collision is elastic.
    bool collide(Body& other, scalar radius);
};

} /* namespace bouncers */

#endif /* BOUNCERS_BODY_H_ */
