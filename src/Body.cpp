#include "Body.hpp"
#include "constants.hpp"
#include <cmath>
#include <utility>

using namespace bouncers;

void Body::tick()
{
    this->x += this->vel_x;
    this->y += this->vel_y;
    this->ang = std::fmod(this->ang + this->vel_ang, TAU);
}

bool Body::collide(Body& other, scalar radius)
{
    scalar dist = std::hypot(this->x - other.x, this->y - other.y);
    if (dist <= radius * 2 && dist > 0) {
        // Rotate the reference frame by -(collision angle).
        scalar cos = (other.x - this->x) / dist;
        scalar sin = -(other.y - this->y) / dist;
        // Collide the bodies.
        scalar this_vel_x_adj = this->vel_x * cos - this->vel_y * sin;
        scalar this_vel_y_adj = this->vel_x * sin + this->vel_y * cos;
        scalar other_vel_x_adj = other.vel_x * cos - other.vel_y * sin;
        scalar other_vel_y_adj = other.vel_x * sin + other.vel_y * cos;
        this->vel_x = other_vel_x_adj * cos - this_vel_y_adj * -sin;
        this->vel_y = other_vel_x_adj * -sin + this_vel_y_adj * cos;
        other.vel_x = this_vel_x_adj * cos - other_vel_y_adj * -sin;
        other.vel_y = this_vel_x_adj * -sin + other_vel_y_adj * cos;
        // Correct the overlap of the bodies.
        scalar correction = radius - dist / 2;
        scalar correction_x = correction * cos;
        scalar correction_y = correction * -sin;
        this->x -= correction_x;
        this->y -= correction_y;
        other.x += correction_x;
        other.y += correction_y;
        // Collide angularly.
        std::swap(this->vel_ang, other.vel_ang);
        return true;
    }
    return false;
}
