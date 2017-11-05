;function PIO
global outb
global outw
global inb
global inw

outb: ; write byte
    enter 0,0
    mov al,[ebp+12]
    mov dx,[ebp+8]
    out dx,al
    leave
    ret

outw: ; write word
    enter 0,0
    mov ax,[ebp+12]
    mov dx,[ebp+8]
    out dx,ax
    leave
    ret

inb: ; read byte
    enter 0,0
    mov dx,[ebp+8]
    in al,dx
    leave
    ret

inw: ; read word
    enter 0,0
    mov dx, [ebp+8]
    in ax,dx
    leave
    ret