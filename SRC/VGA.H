
#ifndef _VGA_H
#define _VGA_H

#include <conio.h>

int switch_to_vga(int mode);
int restore_mode(int mode);
void vga_vsync(void);
void vga_setup_copy(void);
void vga_select_planes(unsigned char planes);
void vga_get_palette(unsigned char *data, unsigned int n);
void vga_set_palette(const unsigned char *data, unsigned int a, unsigned int n);

#pragma aux switch_to_vga = \
    "cld" \
    "and eax, 07fh" \
    "push ax" \
    "mov ah, 0fh" \
    "int 10h" \
    "and ax, 07fh" \
    "mov dx, ax" \
    "pop ax" \
    "mov cl, al" \
    "int 10h" \
    "mov ah, 0fh" \
    "int 10h" \
    "and al, 07fh" \
    "cmp al, cl" \
    "je video_change_ok" \
    "mov dx, 0ffffh" \
    "video_change_ok:" \
    parm [eax] value [edx] modify exact [eax ebx ecx edx]

#pragma aux restore_mode = \
    "and ax, 07fh" \
    "int 10h" \
    parm [eax] value [eax] modify exact [eax ebx]

#pragma aux vga_vsync = \
	"mov dx,03dah" \
    "vga1:" \
    "in al,dx" \
    "and al,8" \
    "jnz vga1" \
    "vga2:" \
    "in al,dx" \
    "and al,8" \
    "jz vga2" \
    modify exact [eax edx]

#pragma aux vga_setup_copy = \
    "mov dx,03ceh" \
    "mov al,05h" \
    "out dx,al" \
    "inc dl" \
    "xor al,al" \
    "out dx,al" \
    \
    "mov dx,03ceh" \
    "mov al,01h" \
    "out dx,al" \
    "inc dl" \
    "xor al,al" \
    "out dx,al" \
    \
    "mov dx,03ceh" \
    "mov al,08h" \
    "out dx,al" \
    "inc dl" \
    "mov al,0ffh" \
    "out dx,al" \
    \
    modify exact [eax edx]

#pragma aux vga_select_planes = \
    "mov dx,03c4h" \
    "mov al,02h" \
    "out dx,al" \
    "inc dl" \
    "mov al,ah" \
    "out dx,al" \
    parm [ah] modify exact [eax edx]

#pragma aux vga_set_palette = \
    "push dx" \
    "mov dx,03c6h" \
    "mov al,0ffh" \
    "out dx,al" \
    "mov dx,03c8h" \
    "pop ax" \
    "out dx,al" \
    "mov dx,03c9h" \
    "loop1:" \
    "lodsb" \
    "out dx,al" \
    "lodsb" \
    "out dx,al" \
    "lodsb" \
    "out dx,al" \
    "loop loop1" \
    parm [esi] [edx] [ecx] modify exact [esi eax ecx edx]

#pragma aux vga_get_palette = \
    "mov dx,03c6h" \
    "mov al,0ffh" \
    "out dx,al" \
    "mov dx,03c8h" \
    "xor ax,ax" \
    "out dx,al" \
    "mov dx,03c9h" \
    "loop1:" \
    "in al,dx" \
    "stosb" \
    "in al,dx" \
    "stosb" \
    "in al,dx" \
    "stosb" \
    "loop loop1" \
    parm [edi] [ecx] modify exact [edi eax ecx edx]

#endif
