//
// Created by matoran on 11/2/17.
//

#include "console.h"

static uchar BACKGROUND_COLOR = COLOR_BLACK;
static uchar FOREGROUND_COLOR = COLOR_GREEN;

void console_init(){
    console_set_background_color(COLOR_LIGHT_GRAY);
    console_set_foreground_color(COLOR_RED);
    position_t pos;
    pos.x = 0;
    pos.y = 0;
    move_cursor(pos);
    //console_clear();
    int t = 0;
    while(1){
        move_cursor(pos);
        console_test(t);
        sleep();
        if(t){
            t = 0;
        }else{
            t = 1;
        }
    }


}

void console_test(int t){
    for (int line = 0; line < HEIGHT; ++line) {
        for (int column = 0; column < WIDTH; ++column) {
            if(line%2 == t){
                if(column%2 == 0){
                    console_set_background_color(COLOR_BLACK);
                }else{
                    console_set_background_color(COLOR_LIGTH_WHITE);
                }
            }else{
                if(column%2 == 0){
                    console_set_background_color(COLOR_LIGTH_WHITE);
                }else{
                    console_set_background_color(COLOR_BLACK);
                }
            }
            print_char(' ');
        }
    }
}

void sleep(){
    for (int i = 0; i < 100000000; ++i) {
        asm("nop");
    }
}

void console_clear(){
    for (int line = 0; line < HEIGHT; ++line) {
        for (int column = 0; column < WIDTH; ++column) {
            print_char(' ');
        }
    }
}

uchar console_background_color(){
    return BACKGROUND_COLOR;
}

void console_set_background_color(uchar color){
    BACKGROUND_COLOR = color;
}

uchar console_foreground_color(){
    return FOREGROUND_COLOR;
}

void console_set_foreground_color(uchar color){
    FOREGROUND_COLOR = color;
}

void print_char(char c){
    position_t pos = read_cursor();
    *((ushort*)(SCREEN+pos.x*2+pos.y*WIDTH*2)) = (ushort)(c | FOREGROUND_COLOR << 8 | BACKGROUND_COLOR << 12);
    pos.x++;
    pos.y = pos.y + (pos.x / WIDTH);
    pos.x %= WIDTH;
    move_cursor(pos);
}

void print_string(char *string){
    while(*string != 0){
        print_char(*(string++));
    }
}

void move_cursor(position_t position){
    uint16_t pos1D = position.x+position.y*WIDTH;
    outb((int*)0x3D4, 0xE);
    outb((int*)0x3D5, (uchar)(pos1D >> 8));
    outb((int*)0x3D4, 0xF);
    outb((int*)0x3D5, (uchar)(pos1D));
}

position_t read_cursor(){
    position_t pos;
    ushort pos1D = 0;
    outb((int*)0x3D4, 0xE);
    pos1D = inb((int*)0x3D5) << 8;
    outb((int*)0x3D4, 0xF);
    pos1D |= inb((int*)0x3D5);
    pos.x = pos1D % WIDTH;
    pos.y = pos1D / WIDTH;
    return pos;
}

/**
 * %c char
 * %s string
 * %d signed decimal integer
 * %x unsigned hexadecimal integer
 * @param fmt
 * @param ...
 */
void printf(char *fmt, ...){
    bool var = false;
    char *base = (void*)&fmt;
    int nb_var = 1;
    int val;
    while(*fmt != 0){
        if(*fmt=='%'){
            var = true;
        }else{
            if(var){
                switch (*fmt){
                    case 'c':
                        print_char(*(base+nb_var*4));
                        break;
                    case 's':
                        print_string((char*)*((int*)(base+nb_var*4)));
                        break;
                    case 'd':
                        print_int(*((int*)(base+nb_var*4)), 10);
                        break;
                    case 'x':
                        print_int(*((int*)(base+nb_var*4)), 16);
                        break;
                }
                nb_var++;
                var = false;
            }else{
                print_char(*(fmt));
            }
        }
        fmt++;

    }
}

//TODO
//
#define ALPHABET "0123456789ABCDEF"
void print_int(int num, uchar base){
    int len = 0;
    char number[11];
    if(num < 0){
        num *= -1;
        print_char('-');
    }
    while(num >= base){
        number[len++] = ALPHABET[num % base];
        num /= base;
    }
    number[len++] = ALPHABET[num];
    number[len] = '\0';
    for(int i=0; i < len/2; i++){
        char temp = number[i];
        number[i] = number[len-i-1];
        number[len-i-1] = temp;
    }
    print_string(number);

}