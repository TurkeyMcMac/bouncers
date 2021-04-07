#ifndef BOUNCERS_MINMAX_H_
#define BOUNCERS_MINMAX_H_

#include "scalar.hpp"

namespace bouncers {

// These definitions work for this program.

template <typename T> constexpr T min(T a, T b) { return a < b ? a : b; }

template <typename T> constexpr T max(T a, T b) { return a > b ? a : b; }

} /* namespace bouncers */

#endif /* BOUNCERS_MINMAX_H_ */
