#include "keyboard.h"
#include "../common/types.h"

static char keys[10];
static uchar pos = 0;

void keyboard_init() {

}

void keyboard_handler() {
    if(keys < 10){
        pos = ++pos % 10;
    }
}

int getc() {
    while(pos == 0){}
    return keys[--pos];
}

// Non-blocking call. Return 1 if a key is pressed
int keypressed() {

}

