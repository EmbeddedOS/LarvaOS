#pragma once
#include <types.h>

#define VIDEO_MEMORY    0xB8000
#define VGA_WIDTH       1000
#define VGA_HEIGHT      40
#define SCREEN_SIZE     (VGA_WIDTH * VGA_HEIGHT)

void video_clear();

void putc(char c);

void putc_with_color(char c, color_t color);

uint16_t get_x();
uint16_t get_y();
void set_x(uint16_t);
void set_x(uint16_t);

