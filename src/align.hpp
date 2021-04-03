#ifndef BOUNCERS_ALIGN_H_
#define BOUNCERS_ALIGN_H_

#include <cstddef>

namespace bouncers {

// This might be wrong, but it's just an optimization.
constexpr int CACHE_LINE_SIZE = 64;

void* aligned_alloc(std::size_t align, std::size_t size, void*& buf);

} /* namespace bouncers */

#endif /* BOUNCERS_ALIGN_H_ */
