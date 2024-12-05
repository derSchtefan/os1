#include <cstdlib>

// no inline, required by [replacement.functions]/3
void* operator new(std::size_t sz)
{
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    return malloc(sz);
}
 
// no inline, required by [replacement.functions]/3
void* operator new[](std::size_t sz)
{
    if (sz == 0)
        ++sz; // avoid std::malloc(0) which may return nullptr on success
 
    return malloc(sz);
}
 
void operator delete(void* ptr) noexcept
{
    free(ptr);
}
 
void operator delete(void* ptr, std::size_t size) noexcept
{
    free(ptr);
}
 
void operator delete[](void* ptr) noexcept
{
    free(ptr);
}
 
void operator delete[](void* ptr, std::size_t size) noexcept
{
    free(ptr);
}
 

namespace std {
    void __throw_bad_array_new_length() {

    }

    void __throw_length_error(char const*){

    }

    void __throw_bad_alloc(){

    }
}
