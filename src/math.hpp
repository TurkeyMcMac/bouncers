#ifndef BOUNCERS_MATH_H_
#define BOUNCERS_MATH_H_

#include "scalar.hpp"

namespace bouncers {

constexpr scalar PI = 3.14159265358979323846;

constexpr scalar TAU = 2 * PI;

struct PolarCoord {
    scalar ang, dist;
};

scalar clamp_angle(scalar ang);

PolarCoord polar_relative(scalar base_ang, scalar rel_x, scalar rel_y);

scalar sigmoid(scalar x);

} /* namespace bouncers */

#endif /* BOUNCERS_MATH_H_ */
