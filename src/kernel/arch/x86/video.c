#include "video.h"
#include <string.h>

uint16_t *g_video_mem = (uint16_t *)VIDEO_MEMORY;
size_t g_row = 0;
size_t g_column = 0;
uint8_t g_color = VGA_COLOR_WHITE;

static void putc(char c);

static void putc_at(char c, uint8_t color, size_t x, size_t y);

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(char c, uint8_t color)
{
    return c | color << 8;
}

void clear()
{
    g_row = 0;
    g_column = 0;
    g_color = VGA_COLOR_WHITE;
    memset(g_video_mem, 0, g_screen_size);
}

void write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        putc(data[i]);
    }
}

static void putc(char c)
{
    putc_at(c, g_color, g_column, g_row);
    if (++g_column == g_vga_width)
    {
        g_column = 0;
        if (++g_row == g_vga_height)
        {
            g_row = 0;
        }
    }
}

static void putc_at(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * g_vga_width + x;
    g_video_mem[index] = vga_entry(c, color);
}

void set_color(enum vga_color c)
{
    g_color = c;
}