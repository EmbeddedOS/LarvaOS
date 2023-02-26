#include <video.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

uint16_t *g_video_mem = (uint16_t *)VIDEO_MEMORY;
size_t g_row = 0;
size_t g_column = 0;
uint8_t g_color = VGA_COLOR_WHITE;

static void putc(char c);

static void putc_at(char c, uint8_t color, size_t x, size_t y);

static void put_special_char(char c);

static inline bool is_end_line_char(char c)
{
    return c == '\n';
}

static inline bool is_backspace_char(char c)
{
    return c == '\b';
}

static inline bool is_special_char(char c)
{
    return is_end_line_char(c) || is_backspace_char(c);
}

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
    if (is_special_char(c))
    {
        put_special_char(c);
        return;
    }

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

static void put_special_char(char c)
{
    switch (c)
    {
    case '\n':
        g_row += 1;
        g_column = 0;
        break;
    case '\b':
        if (g_column == 0 && g_row == 0)
        {
            break;
        }

        if (g_column == 0)
        {
            g_row -= 1;
            g_column = g_vga_width;
        }

        g_column -= 1;
        putc(' ');
        g_column -= 1;

        break;
    default:

        break;
    }
}

void set_color(enum vga_color c)
{
    g_color = c;
}

void print(const char *data)
{
    write(data, strlen(data));
}

void print_number(int x)
{
    char str[32];
    itoa(x, str, 10);
    print(str);
}