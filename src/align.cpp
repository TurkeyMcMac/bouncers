#include "align.hpp"
#include <cstdint>
#include <cstdlib>

namespace bouncers {

void* aligned_alloc(std::size_t align, std::size_t size, void*& buf)
{
    buf = nullptr;
    std::size_t buf_size = size + align;
    if (buf_size <= size)
        return nullptr;
    buf = std::malloc(buf_size);
    if (!buf)
        return nullptr;
    std::uintptr_t buf_addr = (std::uintptr_t)buf;
    std::uintptr_t aligned_addr = (buf_addr + align - 1) & ~(align - 1);
    return (void*)aligned_addr;
}

} /* namespace bouncers */
