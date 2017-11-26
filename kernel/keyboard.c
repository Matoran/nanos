/**
 * @authors: LOPES Marco, ISELI Cyril
 * Purpose: Manage keyboard, support only swiss french keyboard
 * Language:  C
 * Date : November 2017
 */
#include "keyboard.h"
#include "../common/types.h"
#include "../common/console.h"
#define KEY_SHIFT 42
#define KEY_SHIFT_LEFT 54
#define BUFFER_SIZE 10
#define STATE_REGISTER_KEYBOARD 0x64
#define DATA_REGISTER_KEYBOARD 0x60

static uchar buffer[BUFFER_SIZE];
static uchar write = 0;
static uchar read = 0;
static uchar count = 0;
static uchar shift = 0;

/**
 * default keymap
 */
static uchar table[128] = {
        //0   1    2    3    4    5    6    7    8    9
        ' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', //0+x
        '9', '0', '\'', '^', '\b', ' ', 'q', 'w', 'e', 'r',//10+x
        't', 'z', 'u', 'i', 'o', 'p', 138, ' ', '\n', ' ',//20+x
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 130,//30+x
        133, ' ', ' ', '$', 'y', 'x', 'c', 'v', 'b', 'n',//40+x
        'm', ',', '.', '-', ' ', ' ', ' ', ' ', ' ', ' ',//50+x
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',//60+x
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',//70+x
        ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', ' ', ' ',//80+x
};

/**
 * keymap when shift is pressed
 */
static uchar shift_table[128] = {
        //0   1    2    3    4    5    6    7    8    9
        ' ', ' ', '+', '"', '*', 135, '%', '&', '/', '(', //0+x
        ')', '=', '?', '`', '\b', ' ', 'Q', 'W', 'E', 'R',//10+x
        'T', 'Z', 'U', 'I', 'O', 'P', 129, '!', '\n', ' ',//20+x
        'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 148,//30+x
        133, ' ', ' ', 156, 'Y', 'X', 'C', 'V', 'B', 'N',//40+x
        'M', ';', ':', '_', ' ', ' ', ' ', ' ', ' ', ' ',//50+x
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',//60+x
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',//70+x
        ' ', ' ', ' ', ' ', ' ', ' ', '>', ' ', ' ', ' ',//80+x
};

void keyboard_init() {

}

/**
 * keyboard irq handler, check if data is available
 * if data then read character and check if it's a break or a make code
 * save character in buffer if it's a make code
 */
void keyboard_handler() {
    if (inb(STATE_REGISTER_KEYBOARD) & 1) {
        uchar character = inb(DATA_REGISTER_KEYBOARD);
        if (character >> 7) {
            //break code
            if (character == KEY_SHIFT + 128 || character == KEY_SHIFT_LEFT + 128) {
                shift--;
            }
        } else {
            //make code
            if (count < BUFFER_SIZE) {
                if (character == KEY_SHIFT || character == KEY_SHIFT_LEFT) {
                    shift++;
                } else {
                    if (shift) {
                        buffer[write++] = shift_table[character];
                    } else {
                        buffer[write++] = table[character];
                    }
                    write %= BUFFER_SIZE;
                    count++;
                }
            } else {
                printf("keyboard buffer is full\n");
            }
        }
    }
}

/**
 * get char from buffer, blocking call if buffer is empty
 * @return char
 */
int getc() {
    while (count == 0) {}
    count--;
    uchar character = buffer[read++];
    read %= BUFFER_SIZE;
    return character;
}

// Non-blocking call. Return 1 if a key is pressed
int keypressed() {
    return count > 0 ? 1 : 0;
}

