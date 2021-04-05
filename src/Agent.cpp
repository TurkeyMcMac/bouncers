#include "Agent.hpp"
#include "math.hpp"
#include <cmath>
#include <cstring>

using namespace bouncers;

void Agent::act(
    Body& self, Body other, scalar time, scalar straight_acc, scalar turn_speed)
{
    scalar in[Agent::BRAIN_IN], out[Agent::BRAIN_OUT];

    // Put the offset from the origin in the input.
    PolarCoord offset = polar_relative(self.ang, 0 - self.x, 0 - self.y);
    in[0] = offset.ang;
    in[1] = offset.dist;
    // Put the velocity relative to the origin in the input.
    in[2] = clamp_angle(std::atan2(0 - self.vel_y, 0 - self.vel_x) - self.ang);
    in[3] = std::hypot(self.vel_x, self.vel_y);
    // Put the relative position in the input.
    PolarCoord other_pos
        = polar_relative(self.ang, other.x - self.x, other.y - self.y);
    in[4] = other_pos.ang;
    in[5] = other_pos.dist;
    // Put the relative velocity in the input.
    scalar rel_vel_x = other.vel_x - self.vel_x;
    scalar rel_vel_y = other.vel_y - self.vel_y;
    in[6] = clamp_angle(std::atan2(rel_vel_y, rel_vel_x) - self.ang);
    in[7] = std::hypot(rel_vel_x, rel_vel_y);
    // Put the relative angle in the input.
    in[8] = clamp_angle(other.ang - self.ang);
    // Put the time in the input.
    in[9] = time;

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
