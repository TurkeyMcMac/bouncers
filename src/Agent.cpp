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

static PolarCoord get_polar_pos(Body body, scalar x, scalar y)
{
    scalar rel_x = x - body.x;
    scalar rel_y = y - body.y;
    PolarCoord coord = { 0, 0 };
    coord.dist = std::hypot(rel_x, rel_y);
    if (coord.dist > 0)
        coord.ang = clamp_angle(std::atan2(rel_y, rel_x) - body.ang);
    return coord;
}

static scalar sigmoid(scalar x)
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

void Agent::act(Body& self, Body other, scalar straight_acc, scalar turn_speed)
{
    scalar in[Agent::BRAIN_IN], out[Agent::BRAIN_OUT];

    // Put the offset from the origin in the input.
    PolarCoord offset = get_polar_pos(self, 0, 0);
    in[0] = offset.ang;
    in[1] = offset.dist;
    // Put the velocity relative to the origin in the input.
    scalar origin_rel_vel_x = 0 - self.vel_x;
    scalar origin_rel_vel_y = 0 - self.vel_y;
    in[2] = clamp_angle(
        std::atan2(origin_rel_vel_y, origin_rel_vel_x) - self.ang);
    in[3] = std::hypot(origin_rel_vel_x, origin_rel_vel_y);
    // Put the relative position in the input.
    PolarCoord other_pos = get_polar_pos(self, other.x, other.y);
    in[4] = other_pos.ang;
    in[5] = other_pos.dist;
    // Put the relative velocity in the input.
    scalar rel_vel_x = other.vel_x - self.vel_x;
    scalar rel_vel_y = other.vel_y - self.vel_y;
    in[6] = clamp_angle(std::atan2(rel_vel_y, rel_vel_x) - self.ang);
    in[7] = std::hypot(rel_vel_x, rel_vel_y);
    // Put the relative angle in the input.
    in[8] = clamp_angle(other.ang - self.ang);

    // Compute.
    this->brain.compute(in, out);
    // Move.
    scalar out_forward = out[0];
    scalar out_turn = out[1];
    scalar forward = sigmoid(out_forward) * straight_acc * 2 - straight_acc;
    self.vel_x += std::cos(self.ang) * forward;
    self.vel_y += std::sin(self.ang) * forward;
    self.ang += sigmoid(out_turn) * turn_speed * 2 - turn_speed;
}
