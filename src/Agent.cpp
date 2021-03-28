#include "Agent.hpp"
#include "constants.hpp"
#include <cmath>
#include <cstring>

using namespace bouncers;

struct PolarCoord {
    scalar ang, dist;
};

static scalar clamp_angle(scalar ang)
{
    // TODO: Is there a better way to do this?
    if (ang > PI) {
        do {
            ang -= TAU;
        } while (ang > PI);
    } else if (ang < -PI) {
        do {
            ang += TAU;
        } while (ang < -PI);
    }
    return ang;
}

static PolarCoord get_polar_pos(const Body& body, scalar x, scalar y)
{
    scalar rel_x = x - body.x;
    scalar rel_y = y - body.y;
    PolarCoord coord = { 0, 0 };
    coord.dist = std::hypot(rel_x, rel_y);
    if (coord.dist > 0)
        coord.ang = clamp_angle(std::atan2(rel_y, rel_x) - body.ang);
    return coord;
}

static scalar sigmoid(scalar x) { return std::exp(x) / (std::exp(x) + 1); }

void Agent::consider_other(const Agent& other, scalar radius)
{
    scalar in[Agent::OTHER_BRAIN_IN], out[Agent::OTHER_BRAIN_OUT];
    std::memcpy(in, &this->memory, sizeof(this->memory));
    PolarCoord rel = get_polar_pos(this->body, other.body.x, other.body.y);
    std::memcpy(&this->memory, out, sizeof(this->memory));
}

void Agent::act() { }
