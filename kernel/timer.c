/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Timer
 * Language:  C
 * Date : November 2017
 */
#include "timer.h"
#include "../common/console.h"

#define MAX_DIV 65535
#define MIN_DIV 1
#define COMMAND_REGISTER_PIT 0x43
#define DATA0_REGISTER_PIT 0x40
#define LOOP_AND_USE_DIVISOR 0x36

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
    outb(COMMAND_REGISTER_PIT, LOOP_AND_USE_DIVISOR);
    outb(DATA0_REGISTER_PIT, div & 0xFF);
    outb(DATA0_REGISTER_PIT, div >> 8);
    printf("Timer initialized\n");
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
