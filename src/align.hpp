#ifndef BOUNCERS_ALIGN_H_
#define BOUNCERS_ALIGN_H_

#include <algorithm>
#include <cstddef>

namespace bouncers {

// 64 might not be the right cache line size, but it's just an optimization.
constexpr int THREAD_SEP_ALIGN = std::max(64, (int)alignof(std::max_align_t));

void* aligned_alloc(std::size_t align, std::size_t size, void*& buf);

} /* namespace bouncers */

#endif /* BOUNCERS_ALIGN_H_ */
