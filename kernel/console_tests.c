/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Tests for the display console
 * Language:  C
 * Date : October/November 2017
 */

#include "console_tests.h"
#include "console.h"
#include "timer.h"

/**
 * Test the scroll
 */
static void scroll() {
    for (int i = 0; i < 30; ++i) {
        printf("%d\n", i);
        if (i > 20) {
            sleep(1000);
        }
    }
    for (int j = 0; j < WIDTH + 2; ++j) {
        printf("a");
        if (j >= WIDTH - 3) {
            sleep(1000);
        }
    }
}

/**
 * Display a blink checkerboard
 * @param black boolean which displays in black or in white
 */
static void checkerboard_blink(bool black) {
    for (int line = 0; line < HEIGHT; ++line) {
        for (int column = 0; column < WIDTH; ++column) {
            if (line % 2 == black) {
                if (column % 2 == 0) {
                    console_set_background_color(COLOR_BLACK);
                } else {
                    console_set_background_color(COLOR_LIGTH_WHITE);
                }
            } else {
                if (column % 2 == 0) {
                    console_set_background_color(COLOR_LIGTH_WHITE);
                } else {
                    console_set_background_color(COLOR_BLACK);
                }
            }
            printf(" ");
        }
    }
    printf("checkerboard blinking");
}

/**
 * Zoom on the checkerboard
 * @param size of zoom
 */
static void checkerboard_zoom(int size) {
    for (int line = 0; line < HEIGHT - 1; ++line) {
        for (int column = 0; column < WIDTH; ++column) {
            if (line / size % 2 == column / size % 2) {
                console_set_background_color((size - 1) % 16);
            } else {
                console_set_background_color(15 - ((size - 1) % 16));
            }
            printf(" ");
        }
    }
    printf("checkerboard zooming");
}

/**
 * show 1 character and change color (loop)
 */
static void color() {
    for (int j = 0; j < 26 * 80 / 12; ++j) {
        for (uchar i = 0; i <= 12; ++i) {
            console_set_background_color(i);
            printf(" ");
            sleep(10);
        }
    }
}

/**
 * launch console tests
 */
void console_tests() {
    bool black = true;
    position_t pos;
    pos.x = 0;
    pos.y = 0;
    scroll();
    for (int i = 0; i < 5; ++i) {
        move_cursor(pos);
        checkerboard_blink(black);
        black = !black;
        sleep(200);
    }
    console_clear();
    for (int i = 1; i <= HEIGHT; ++i) {
        move_cursor(pos);
        checkerboard_zoom(i);
        sleep(200);
    }
    move_cursor(pos);
    color();
}
