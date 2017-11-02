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
    console_clear();

    move_cursor(pos);
    print_string("abcdefghijklmnopqrstuvwxyz 0123456789 ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    printf(":%s:%c:%d:%d:", "string test printf", 'A', 42, -42);
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
                        val = *((int*)(base+nb_var*4));
                        if(val < 0){
                            print_int(val);
                            print_string("negatif");
                            print_char('-');
                        }else{
                            print_int(val);
                            print_string("positif");
                        }
                        break;
                    case 'x':

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
void print_int(int num){
    int len = 0;
    int shift = 3;
    char number[11];
    while(num >= 10){
        number[len] = (char)((num % 10)+48);
        num /= 10;
        len++;
        shift++;
    }
    number[len] = '\0';
    print_string(number);

}