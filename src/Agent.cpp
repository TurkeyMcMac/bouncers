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

void Agent::consider_other(const Agent& other)
{
    scalar in[Agent::OTHER_BRAIN_IN], out[Agent::OTHER_BRAIN_OUT];
    // Put the memory in the input.
    std::memcpy(in, &this->memory, sizeof(this->memory));
    // Put the relative position in the input.
    PolarCoord other_pos
        = get_polar_pos(this->body, other.body.x, other.body.y);
    in[Agent::MEMORY_SIZE + 0] = other_pos.ang;
    in[Agent::MEMORY_SIZE + 1] = other_pos.dist;
    // Put the relative velocity in the input.
    scalar rel_vel_x = other.body.vel_x - this->body.vel_x;
    scalar rel_vel_y = other.body.vel_y - this->body.vel_y;
    in[Agent::MEMORY_SIZE + 2]
        = clamp_angle(std::atan2(rel_vel_y, rel_vel_x) - this->body.ang);
    in[Agent::MEMORY_SIZE + 3] = std::hypot(rel_vel_x, rel_vel_y);
    // Put the relative angle in the input.
    in[Agent::MEMORY_SIZE + 4] = clamp_angle(other.body.ang - this->body.ang);
    // Put the angular velocity in the input.
    in[Agent::MEMORY_SIZE + 5] = other.body.vel_ang;
    // Compute.
    this->other_brain.compute(in, out);
    // Remember.
    std::memcpy(&this->memory, out, sizeof(this->memory));
}

void Agent::act(scalar forward_speed, scalar turn_speed)
{
    scalar in[Agent::OTHER_BRAIN_IN], out[Agent::OTHER_BRAIN_OUT];
    // Make sure no dangerous values are present in memory.
    for (int i = 0; i < Agent::MEMORY_SIZE; ++i) {
        if (this->memory[i] < Agent::MEMORY_VAL_MIN) {
            this->memory[i] = Agent::MEMORY_VAL_MIN;
        } else if (this->memory[i] > Agent::MEMORY_VAL_MAX) {
            this->memory[i] = Agent::MEMORY_VAL_MAX;
        }
    }
    // Put the memory in the input.
    std::memcpy(in, &this->memory, sizeof(this->memory));
    // Put the offset from the origin in the input.
    PolarCoord offset = get_polar_pos(this->body, 0, 0);
    in[Agent::MEMORY_SIZE + 0] = offset.ang;
    in[Agent::MEMORY_SIZE + 1] = offset.dist;
    // Put the velocity relative to the origin in the input.
    scalar rel_vel_x = 0 - this->body.vel_x;
    scalar rel_vel_y = 0 - this->body.vel_y;
    in[Agent::MEMORY_SIZE + 2]
        = clamp_angle(std::atan2(rel_vel_y, rel_vel_x) - this->body.ang);
    in[Agent::MEMORY_SIZE + 3] = std::hypot(rel_vel_x, rel_vel_y);
    // Put the angular velocity in the input.
    in[Agent::MEMORY_SIZE + 4] = this->body.vel_ang;
    // Compute.
    this->self_brain.compute(in, out);
    // Remember.
    std::memcpy(&this->memory, out, sizeof(this->memory));
    // Move.
    scalar out_forward = out[Agent::MEMORY_SIZE + 0];
    scalar out_turn = out[Agent::MEMORY_SIZE + 1];
    scalar forward = sigmoid(out_forward) * forward_speed * 2 - forward_speed;
    this->body.vel_x += std::cos(this->body.ang) * forward;
    this->body.vel_y += std::sin(this->body.ang) * forward;
    this->body.vel_ang += sigmoid(out_turn) * turn_speed * 2 - turn_speed;
}
