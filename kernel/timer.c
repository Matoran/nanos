/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Timer
 * Language:  C
 * Date : November 2017
 */
#include "timer.h"
#include "../common/console.h"

static uint ticks = 0;
static uint32_t frequency;

/**
 * initialize timer with freq_hz
 * @param freq_hz
 */
void timer_init(uint32_t freq_hz) {
    ushort div;
    if (freq_hz > MAX_FREQ) {
        div = MIN_DIV;
        frequency = MAX_FREQ;
    } else if (freq_hz < MIN_FREQ) {
        div = MAX_DIV;
        frequency = MIN_FREQ;
    } else {
        div = MAX_FREQ / freq_hz;
        frequency = freq_hz;
    }
    outb(0x43, 0x36);
    outb(0x40, div & 0xFF);
    outb(0x40, div >> 8);
}

/**
 * called each time timer finish
 */
void timer_handler() {
    ticks++;
}

/**
 * return current ticks
 * @return current ticks
 */
uint get_ticks() {
    return ticks;
}

/**
 * sleep x ms
 * @param ms
 */
void sleep(uint ms) {
    uint32_t end = ticks + ms * frequency / 1000;
    while (ticks <= end);
}
