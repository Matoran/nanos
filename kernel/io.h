//
// Created by matoran on 12/13/17.
//

#ifndef NANOS_IO_H
#define NANOS_IO_H

#include "../common/types.h"

extern void outb(uint16_t port, uint8_t data);
extern void outw(uint16_t port, uint16_t data);
extern uchar inb(uint16_t port);
extern ushort inw(uint16_t port);

#endif //NANOS_IO_H
