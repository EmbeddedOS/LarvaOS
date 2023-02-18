#pragma once
#include <types.h>

void *memset(void *str, int c, size_t size);
int memcmp(const void *str1, const void *str2, size_t n);
void *memcpy(void *d, const void *s, size_t n);

size_t strlen(const char *s);
size_t strnlen(const char *s, size_t count);
char *strchr(const char *s, int c);
char* strcpy(char* d, const char* s);
