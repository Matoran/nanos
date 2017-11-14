#include "keyboard.h"
#include "../common/types.h"
#include "../common/console.h"
#define KEY_SHIFT 42

static char keys[10];
static uchar pos = 0;
static bool shift = false;

static uchar table[128] = {
        //0   1    2    3    4    5    6    7    8    9
        ' ', ' ', '1', '2', '3', '4', '5', '6', '7', '8', //0+x
        '9', '0', '\'', '^', ' ', ' ', 'q', 'w', 'e', 'r',//10+x
        't', 'z', 'u', 'i', 'o', 'p', 138, ' ', '\n', ' ',//20+x
        'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 130,//30+x
        133, ' ', ' ', '$', 'y', 'x', 'c', 'v', 'b', 'n',//40+x
        'm', ',', '.', '-', ' ', ' ', ' ', ' ', ' ', ' ',//50+x
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',//60+x
        ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',//70+x
        ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', ' ', ' ',//80+x
};

static uchar shift_table[128] = {
        //0   1    2    3    4    5    6    7    8    9
        ' ', ' ', '+', '"', '*', 135, '%', '&', '/', '(', //0+x
        ')', '=', '?', '`', ' ', ' ', 'Q', 'W', 'E', 'R',//10+x
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

void keyboard_handler() {
    if(inb(0x64) & 1){
        if(pos < 10){
            uchar character = inb(0x60);
            if(character >> 7){
                if(character == KEY_SHIFT+128){
                    shift = false;
                }
                //break code
                //printf("break code %d\n", character);
            }else{
                //make code
                if(character == KEY_SHIFT){
                    shift = true;
                }
                if(shift){
                    printf("%c", shift_table[character]);
                }else{
                    printf("%c", table[character]);

                }
            }
            //keys[pos++] = ;
        }else{
            printf("keyboard buffer is full");
        }
    }
}

int getc() {
    while(pos == 0){}
    return keys[--pos];
}

// Non-blocking call. Return 1 if a key is pressed
int keypressed() {

}

