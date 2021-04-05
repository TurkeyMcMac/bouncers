#include "align.hpp"
#include <cstdlib>
#include <memory>

namespace bouncers {

void* aligned_alloc(std::size_t align, std::size_t size, void*& buf)
{
    void* ptr;
    std::size_t buf_size = size + align;
    if (buf_size <= size)
        goto error;
    buf = std::malloc(buf_size);
    if (!buf)
        goto error;
    ptr = buf;
    if (!std::align(align, size, ptr, buf_size)) {
        std::free(buf);
        goto error;
    }
    return ptr;

error:
    buf = nullptr;
    return nullptr;
}

} /* namespace bouncers */
