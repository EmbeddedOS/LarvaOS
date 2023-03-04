#pragma once
#include <types.h>

[[gnu::visibility("default")]] void *operator new(size_t size);
[[gnu::visibility("default")]] void operator delete(void *ptr);
[[gnu::visibility("default")]] void *operator new[](size_t size);
[[gnu::visibility("default")]] void operator delete[](void *ptr);
[[gnu::visibility("default")]] void operator delete(void *ptr, size_t size);
[[gnu::visibility("default")]] void operator delete[](void *ptr, size_t size);
