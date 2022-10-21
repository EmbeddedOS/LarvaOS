#include "video.h"
#include <string.h>

uint16_t *g_video_mem = (uint16_t *)VIDEO_MEMORY;
uint16_t g_x_cursor = 0;
uint16_t g_y_cursor = 0;

void video_clear()
{
    //g_x_cursor = 0;
    //g_y_cursor = 0;
    memset(g_video_mem, 0, SCREEN_SIZE);
          //    unsigned short *g_video_mem = (unsigned short *)0xB8000;
          //  for (int i = 0; i < 1000; i++)
         //    g_video_mem[i] = (0 << 8) | ' ';
        // for (int i = 0; i < 100; i++)
          //   g_video_mem[i] = (2 << 8) | 'A';
}

void putc(char c)
{
}

void putc_with_color(char c, color_t color)
{
    g_video_mem[1] = (color << 8) | c;
}


uint16_t get_x()
{
    return g_x_cursor;
}

uint16_t get_y()
{
    return g_y_cursor;
}

void set_x(uint16_t x)
{
    if (x <= VGA_WIDTH)
    {
        g_x_cursor = x;
    }
}

void set_y(uint16_t y)
{
    if (y <= VGA_HEIGHT)
    {
        g_x_cursor = y;
    }
}
