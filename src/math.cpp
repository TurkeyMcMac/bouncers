#include "math.hpp"
#include <cmath>

namespace bouncers {

scalar clamp_angle(scalar ang)
{
    ang = std::fmod(ang, TAU);
    if (ang > PI) {
        ang -= TAU;
    } else if (ang < -PI) {
        ang += TAU;
    }
    return ang;
}

PolarCoord polar_relative(scalar base_ang, scalar rel_x, scalar rel_y)
{
    PolarCoord coord = { 0, 0 };
    coord.dist = std::hypot(rel_x, rel_y);
    if (coord.dist > 0)
        coord.ang = clamp_angle(std::atan2(rel_y, rel_x) - base_ang);
    return coord;
}

scalar sigmoid(scalar x)
{
    // Only small x values are passed to the function, to prevent overflow.
    if (x > 15) {
        return 1;
    } else if (x < -15) {
        return 0;
    } else {
        scalar exp = std::exp(x);
        return exp / (exp + 1);
    }
}

} /* namespace bouncers */
