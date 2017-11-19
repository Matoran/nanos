/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Management for display console
 * Language:  C
 * Date : October/November 2017
 */
#include "../common/console.h"
#include "../common/memory.h"

#define ALPHABET "0123456789ABCDEF"
static uchar BACKGROUND_COLOR = COLOR_BLACK;
static uchar FOREGROUND_COLOR = COLOR_GREEN;

static ushort read_cursor_1D();

static void move_cursor_1D(ushort pos1D);

/**
 * Scroll the graphics console
 */
static void scroll() {
    memcpy((ushort *) SCREEN, (ushort *) (SCREEN + WIDTH * 2), (HEIGHT - 1) * WIDTH * 2);
    for (int i = 0; i < WIDTH; ++i) {
        *((ushort *) (SCREEN + i * 2 + (HEIGHT - 1) * WIDTH * 2)) = (ushort) (' ' | FOREGROUND_COLOR << 8 |
                                                                              BACKGROUND_COLOR << 12);
    }
    position_t pos;
    pos.x = 0;
    pos.y = HEIGHT - 1;
    move_cursor(pos);
}

/**
 * Print char into the console
 * @param c char we want print
 */
static void print_char(uchar c) {
    position_t pos = read_cursor();
    if (c == '\n') {
        pos.y++;
        if (pos.y >= HEIGHT) {
            scroll();
        } else {
            pos.x = 0;
            move_cursor(pos);
        }
    } else if (c == '\b') {
        ushort pos1D = read_cursor_1D();
        if (pos1D > 0) {
            pos1D--;
            *((ushort *) (SCREEN + pos1D * 2)) = (ushort) (' ' | FOREGROUND_COLOR << 8 |
                                                           BACKGROUND_COLOR << 12);
            move_cursor_1D(pos1D);
        }
    } else {
        *((ushort *) (SCREEN + pos.x * 2 + pos.y * WIDTH * 2)) = (ushort) (c | FOREGROUND_COLOR << 8 |
                                                                           BACKGROUND_COLOR << 12);
        pos.x++;
        pos.y = pos.y + (pos.x / WIDTH);
        pos.x %= WIDTH;
        if (pos.y >= HEIGHT) {
            scroll();
        } else {
            move_cursor(pos);
        }
    }

}

/**
 * Print string into the console
 * @param string string we want print
 */
static void print_string(char *string) {
    while (*string != 0) {
        print_char(*(string++));
    }
}

/**
 * Print an number into the console
 * @param num number we want print
 * @param base of number
 */
static void print_int(int num, uchar base) {
    if (base != 10 && base != 16) {
        printf("base %d not supported", base);
        return;
    }
    int len = 0;
    char number[11];
    if (num < 0) {
        num *= -1;
        print_char('-');
    }
    while (num >= base) {
        number[len++] = ALPHABET[num % base];
        num /= base;
    }
    number[len++] = ALPHABET[num];
    number[len] = '\0';
    for (int i = 0; i < len / 2; i++) {
        char temp = number[i];
        number[i] = number[len - i - 1];
        number[len - i - 1] = temp;
    }
    print_string(number);

}

/**
 * Initialise the console
 */
void console_init() {
    console_set_background_color(COLOR_LIGHT_GRAY);
    console_set_foreground_color(COLOR_RED);
    console_clear();
}

/**
 * Sleep a moment
 * @param multiplier for sleep longer
 */
void sleepOld(float multiplier) {
    for (int i = 0; i < 1000000 * multiplier; ++i) {
        asm("nop");
    }
}

/**
 * Clear the console
 */
void console_clear() {
    position_t pos;
    pos.x = 0;
    pos.y = 0;
    move_cursor(pos);
    for (int line = 0; line < HEIGHT; ++line) {
        for (int column = 0; column < WIDTH; ++column) {
            print_char(' ');
        }
    }
    move_cursor(pos);
}

/**
 * Get the background color of the console
 * @return the color
 */
uchar console_background_color() {
    return BACKGROUND_COLOR;
}

/**
 * Set the background color of the console
 * @param color
 */
void console_set_background_color(uchar color) {
    BACKGROUND_COLOR = color;
}

/**
 * Get the foreground color of the text
 * @return the color
 */
uchar console_foreground_color() {
    return FOREGROUND_COLOR;
}

/**
 * Set the foreground color of the text
 * @param color
 */
void console_set_foreground_color(uchar color) {
    FOREGROUND_COLOR = color;
}

ushort read_cursor_1D() {
    ushort pos1D = 0;
    outb(0x3D4, 0xE);
    pos1D = inb(0x3D5) << 8;
    outb(0x3D4, 0xF);
    pos1D |= inb(0x3D5);
    return pos1D;
}

/**
 * Read the cursor position
 * @return position
 */
position_t read_cursor() {
    position_t pos;
    ushort pos1D = read_cursor_1D();
    pos.x = pos1D % WIDTH;
    pos.y = pos1D / WIDTH;
    return pos;
}

void move_cursor_1D(ushort pos1D) {
    outb(0x3D4, 0xE);
    outb(0x3D5, (uchar) (pos1D >> 8));
    outb(0x3D4, 0xF);
    outb(0x3D5, (uchar) (pos1D));
}

/**
 * Move the cursor
 * @param position
 */
void move_cursor(position_t position) {
    ushort pos1D = position.x + position.y * WIDTH;
    move_cursor_1D(pos1D);
}

/**
 * Printf
 * %c char
 * %s string
 * %d signed decimal integer
 * %x unsigned hexadecimal integer
 * @param fmt format
 * @param ... all variables
 */
void printf(char *fmt, ...) {
    bool var = false;
    char *base = (void *) &fmt;
    int nb_var = 1;
    while (*fmt != 0) {
        if (*fmt == '%') {
            var = true;
        } else {
            if (var) {
                switch (*fmt) {
                    case 'c':
                        print_char(*(base + nb_var * 4));
                        break;
                    case 's':
                        print_string((char *) *((int *) (base + nb_var * 4)));
                        break;
                    case 'd':
                        print_int(*((int *) (base + nb_var * 4)), 10);
                        break;
                    case 'x':
                        print_int(*((int *) (base + nb_var * 4)), 16);
                        break;
                    default:
                        print_char('%');
                        print_char(*fmt);
                        nb_var--;
                }
                nb_var++;
                var = false;
            } else {
                print_char(*(fmt));
            }
        }
        fmt++;

    }
}
