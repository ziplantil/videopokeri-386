
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

// piirtää kortin selkäpuolen koordinaatteihin (x pyöristyy alas 8 monikertaan)
void piirra_selka(short x, short y);
// piirtää kortin koordinaatteihin (x pyöristyy alas 8 monikertaan)
void piirra_kortti(short x, short y, kortti_t kortti);
// piirtää kortin vasemman ylänurkan (numero, maa)
// jos väri != 0, pakottaa tietyn värin
void piirra_kortti_vynurkka(short x, short y, kortti_t kortti,
                            unsigned char color);
// piirtää kortin selkäpuolen koordinaatteihin
// x1 ja x2 on minimi/maksimi X, mahdollisesti osin peitetty kortti
void piirra_selka_peitetty(short x1, short x2, short y);
// pyyhkii kortin kokoisen reiän näytölle
void pyyhi_kortti(short x, short y);

// siirtää tyhjää aluetta
void siirra_tyhja(short x1, short y1, short x2, short y2);
// siirtää kortin selkäpuolta
void siirra_selka(short x1, short y1, short x2, short y2);
// siirtää korttia
void siirra_kortti(short x1, short y1, short x2, short y2, kortti_t kortti);

#endif
