#pragma once
#include <types.h>

void *memset(void *str, int c, size_t size);
int memcmp(const void *str1, const void *str2, size_t n);

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t count);