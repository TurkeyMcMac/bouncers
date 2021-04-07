#ifndef BOUNCERS_ALIGN_H_
#define BOUNCERS_ALIGN_H_

#include "minmax.hpp"
#include <cstddef>

namespace bouncers {

// THREAD_SEP_ALIGN is the alignment data should have to avoid false sharing (I
// think?) 64 might not be the right cache line size, but it's just an
// optimization.
constexpr int THREAD_SEP_ALIGN = max(64, (int)alignof(std::max_align_t));

// Allocates some memory with the given alignment and size. The memory is
// returned. The memory should not be freed. buf is set to a memory buffer
// contains the returned memory. To free the returned memory, call free(buf).
// If allocation fails, buf is set to nullptr and nullptr is returned.
void* aligned_alloc(std::size_t align, std::size_t size, void*& buf);

} /* namespace bouncers */

#endif /* BOUNCERS_ALIGN_H_ */
