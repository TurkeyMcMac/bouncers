#include "math.hpp"
#include <cmath>

namespace bouncers {

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
