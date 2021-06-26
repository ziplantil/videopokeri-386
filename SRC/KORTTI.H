
#ifndef _KORTTI_H
#define _KORTTI_H

#include "VGA.H"

typedef unsigned char kortti_t;

#define YOFFSET 80
#define COMPUTE_COORD(x, y, o, b) \
    (o = (((x) >> 3) + (y) * YOFFSET), b = ((x) & 7))

#define KARVO(k) ((k) & 15)
#define KMAA(k) (((k) >> 4) & 3)

#define KORTTI_L 96
#define KORTTI_K 128
#define ARVOT_N 13

#define K_JOKERI 0x3F
#define K_SELKA 0xFF

#define ARRAY_SWAP(a, i, j, temp) do {     \
        temp = a[i];                       \
        a[i] = a[j];                       \
        a[j] = temp;                       \
    } while (0);

void piirra_selka(short x, short y);
void piirra_kortti(short x, short y, kortti_t kortti);
void piirra_selka_peitetty(short x1, short x2, short y);
void pyyhi_kortti(short x, short y);
void siirra_tyhja(short x1, short y1, short x2, short y2);
void siirra_selka(short x1, short y1, short x2, short y2);
void siirra_kortti(short x1, short y1, short x2, short y2, kortti_t kortti);

#endif
