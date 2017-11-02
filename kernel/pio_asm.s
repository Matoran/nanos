global outb
global outw
global inb
global inw

outb:
    enter 0,0
    mov al,[ebp+12]
    mov dx,[ebp+8]
    out dx,al

    leave
    ret

outw:
    enter 0,0
    mov ax,[ebp+8]
    mov dx,[ebp+12]
    out dx,ax
    leave
    ret

inb:
    enter 0,0
    mov dx,[ebp+8]
    in al,dx
    leave
    ret

inw:
    enter 0,0
    mov dx, [ebp+8]
    in ax,dx
    leave
    ret