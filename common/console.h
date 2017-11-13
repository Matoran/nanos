#ifndef NANOS_CONSOLE_H
#define NANOS_CONSOLE_H

#include "types.h"

#define COLOR_BLACK 0
#define COLOR_BLUE 1
#define COLOR_GREEN 2
#define COLOR_CYAN 3
#define COLOR_RED 4
#define COLOR_MAGENTA 5
#define COLOR_BROWN 6
#define COLOR_LIGHT_GRAY 7
#define COLOR_DARK_GRAY 8
#define COLOR_LIGHT_BLUE 9
#define COLOR_LIGTH_GREEN 10
#define COLOR_LIGTH_CYAN 11
#define COLOR_LIGTH_RED 12
#define COLOR_LIGTH_MAGENTA 13
#define COLOR_LIGTH_YELLOW 14
#define COLOR_LIGTH_WHITE 15
#define WIDTH (uchar)80
#define HEIGHT (uchar)25
#define SCREEN 0xB8000

typedef struct position_st {
    uchar x;
    uchar y;
} position_t;

extern void console_init();

extern void console_clear();

extern uchar console_background_color();

extern void console_set_background_color(uchar color);

extern uchar console_foreground_color();

extern void console_set_foreground_color(uchar color);

extern void sleep(float multiplier);

extern position_t read_cursor();

extern void move_cursor(position_t position);

extern void printf(char *fmt, ...);

extern void outb(int *address, uchar value);

extern void outw(int *address, ushort value);

extern uchar inb(int *address);

extern ushort inw(int *address);


#endif
