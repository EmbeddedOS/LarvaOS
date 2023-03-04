#include <memory.hh>
extern "C"
{
#include <memory/kheap.h>
}

[[gnu::visibility("default")]] void *operator new(size_t size)
{
    return kmalloc(size);
}

[[gnu::visibility("default")]] void operator delete(void *ptr)
{
    kfree(ptr);
}

[[gnu::visibility("default")]] void *operator new[](size_t size)
{
    return ::operator new(size);
}

[[gnu::visibility("default")]] void operator delete[](void *ptr)
{
    return ::operator delete(ptr);
}

[[gnu::visibility("default")]] void operator delete(void *ptr, size_t size)
{
    ::operator delete(ptr);
}

[[gnu::visibility("default")]] void operator delete[](void *ptr, size_t size)
{
    ::operator delete(ptr);  
}
