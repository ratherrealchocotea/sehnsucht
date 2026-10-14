;; pic.asm
global _disable_int
global _init_pic
global _disable_pic

%define ICW1      0x11
%define PIC1_CTRL 0x20
%define PIC1_DATA 0x21
%define PIC2_CTRL 0xA0
%define PIC2_DATA 0xA1
%define IRQ_0     0x20
%define IRQ_8     0x28

_disable_int:
        cli
        ret

_init_pic:
        mov al, ICW1
        out PIC1_CTRL, al
        out PIC2_CTRL, al

        mov al, IRQ_0
        out PIC1_DATA, al

        mov al, IRQ_8
        out PIC2_DATA, al

        mov al, 0x04
        out PIC1_DATA, al

        mov al, 0x02
        out PIC2_DATA, al

        mov al, 0x01
        out PIC1_DATA, al
        out PIC2_DATA, al

        ret

_disable_pic:   
    mov al, 0xff
    out PIC1_DATA, al

    mov al, 0xff
    out PIC2_DATA, al
    ret
