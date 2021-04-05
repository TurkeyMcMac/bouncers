#ifndef BOUNCERS_MATH_H_
#define BOUNCERS_MATH_H_

#include "scalar.hpp"

namespace bouncers {

// Misc. math stuff.

constexpr scalar PI = 3.14159265358979323846;

constexpr scalar TAU = 2 * PI;

// A polar coordinate.
struct PolarCoord {
    scalar ang, dist;
};

// Returns an angle equivalent to ang in the range [-PI, PI].
scalar clamp_angle(scalar ang);

// Returns a polar coordinate relative to an observer. base_ang is the angle the
// observer is facing, and the angle from which the coordinate will be offset.
// rel_x and rel_y are the Cartesian position offset of the returned. The angle
// of the polar coordinate will be in the range [-PI, PI].
PolarCoord polar_relative(scalar base_ang, scalar rel_x, scalar rel_y);

// Returns an approximation of 1 / (1 + e^-x) for the purposes of softly
// clamping x into [0, 1].
scalar sigmoid(scalar x);

} /* namespace bouncers */

#endif /* BOUNCERS_MATH_H_ */
