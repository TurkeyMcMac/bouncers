#ifndef BOUNCERS_CONSTANTS_H_
#define BOUNCERS_CONSTANTS_H_

#include "scalar.hpp"

namespace bouncers {

// This might be wrong, but it's just an optimization.
constexpr int CACHE_LINE_SIZE = 64;

constexpr scalar PI = 3.14159265358979323846;

constexpr scalar TAU = 2 * PI;

} /* namespace bouncers */

#endif /* BOUNCERS_CONSTANTS_H_ */
