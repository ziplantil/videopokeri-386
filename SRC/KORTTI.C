
#include <dos.h>
#include "MUISTI.H"
#include "POKERI.H"
#include "KORTTI.H"
#include "RUUTU.H"
#include "KUVAT.H"

const unsigned char corner_l[8] =
    { 0x00, 0x07, 0x1F, 0x3F, 0x3F, 0x7F, 0x7F, 0x7F };
const unsigned char corner_r[8] =
    { 0x00, 0xE0, 0xF8, 0xFC, 0xFC, 0xFE, 0xFE, 0xFE };
const unsigned char corner_lx[8] =
    { 0x7F, 0x7F, 0x7F, 0x3F, 0x3F, 0x1F, 0x07, 0x00 };
const unsigned char corner_rx[8] =
    { 0xFE, 0xFE, 0xFE, 0xFC, 0xFC, 0xF8, 0xE0, 0x00 };

#define KAIKKI_TASOT_OR(o, v) \
    (oo = (o), taso0[oo] |= v, taso1[oo] |= v, taso2[oo] |= v, taso3[oo] |= v)
#define KAIKKI_TASOT_MEMSET(o, b, c) \
    (oo = (o), fast_memset(taso0 + oo, b, c), fast_memset(taso1 + oo, b, c), \
               fast_memset(taso2 + oo, b, c), fast_memset(taso3 + oo, b, c))
#define LAKANA_OIKEA ((KORTTI_L - 1) / PPB)
#define LAKANA_ALAS ((KORTTI_K - 8) * STRIDE)

const int max_offset = CARDAREA_HEIGHT * STRIDE;
int offset, bitoffset;

void piirra_lakana(int offset) {
    int i, o = offset;
    short oo;
    for (i = 0; i < 8; ++i) {
        if (o >= max_offset) break;
        RANGE_TASO0(taso0 + o);
        KAIKKI_TASOT_OR(o, corner_l[i]);
        KAIKKI_TASOT_MEMSET(o + 1, 0xff, KORTTI_L / PPB - 2);
        KAIKKI_TASOT_OR(o + LAKANA_OIKEA, corner_r[i]);
        o += STRIDE;
    }
    for (i = 0; i < KORTTI_K - 16; ++i) {
        if (o >= max_offset) break;
        RANGE_TASO0(taso0 + o);
        KAIKKI_TASOT_MEMSET(o, 0xff, KORTTI_L / PPB);
        o += STRIDE;
    }
    for (i = 0; i < 8; ++i) {
        if (o >= max_offset) break;
        RANGE_TASO0(taso0 + o);
        KAIKKI_TASOT_OR(o, corner_lx[i]);
        KAIKKI_TASOT_MEMSET(o + 1, 0xff, KORTTI_L / PPB - 2);
        KAIKKI_TASOT_OR(o + LAKANA_OIKEA, corner_rx[i]);
        o += STRIDE;
    }
}

void piirra_selka(short x, short y) {
    COMPUTE_COORD(x, y, offset, bitoffset);
    piirra_lakana(offset);
    piirra_kuva_kortti(x + 8, y + 8, 80, 112, selka_cache);
}

void piirra_selka_peitetty(short x1, short x2, short y) {
    short xo, i, oo;
    int o;
    x1 &= ~7, x2 &= ~7;
    xo = x2 - x1;
    if (xo >= (KORTTI_L - 8) || xo <= -KORTTI_L + 8)
        piirra_selka(x1, y);
    else if (xo > 0) {
        /* piirrä vasen reuna */
        o = y * STRIDE + (x1 >> 3);
        for (i = 0; i < 8; ++i) {
            KAIKKI_TASOT_OR(o, corner_l[i]);
            KAIKKI_TASOT_OR(o + LAKANA_ALAS, corner_lx[i]);
            o += STRIDE;
        }
        piirra_suorakulmio(x1, y + 8, 8, KORTTI_K - 16, 15);
        piirra_suorakulmio(x1 + 8, y, xo, KORTTI_K, 15);
        if (xo > 8) {
            piirra_kuva_rajaa(x1 + 8, y + 8, xo - 8, 112,
                                0, 0, 80, 112, 
                                selka_cache);
        }
    } else if (xo < 0) {
        /* piirrä oikea reuna */
        short xm = x1 + KORTTI_L - 8, xc;
        piirra_suorakulmio(xm + xo, y, -xo, KORTTI_K, 15);
        o = y * STRIDE + (xm >> 3);
        for (i = 0; i < 8; ++i) {
            KAIKKI_TASOT_OR(o, corner_r[i]);
            KAIKKI_TASOT_OR(o + LAKANA_ALAS, corner_rx[i]);
            o += STRIDE;
        }
        piirra_suorakulmio(xm, y + 8, 8, KORTTI_K - 16, 15);
        if (xo < -8) {
            xc = xm + 8 + xo;
            piirra_kuva_rajaa(xc, y + 8, xm - xc, 112,
                                80 - (xm - xc), 0, 80, 112, 
                                selka_cache);
        }
    }
}

const unsigned char maa_red[] = { 0x00, 0xFF, 0xFF, 0x00 };

void piirra_kortti_frame(int offset, unsigned char red) {
    int i;
    unsigned char *p0, *p1, *p2, *p3, l = 0xFE | red, r = 0x7F | red;

    offset += 23 * STRIDE + 2;
    p0 = taso0 + offset, p1 = taso1 + offset;
    p2 = taso2 + offset, p3 = taso3 + offset;

    RANGE_TASO0(p0);
    for (i = 0; i < 82; ++i)
    {
        *p0 = *p1 = 0xFE;
        *p2 = *p3 = l;
        *(p0 + 7) = *(p1 + 7) = 0x7F;
        *(p2 + 7) = *(p3 + 7) & r;
        p0 += STRIDE, p1 += STRIDE, p2 += STRIDE, p3 += STRIDE;
    }
    ++offset;

    RANGE_TASO0(taso0 + offset);
    fast_memset(taso0 + offset, 0, 6);
    fast_memset(taso1 + offset, 0, 6);
    fast_memset(taso2 + offset, red, 6);
    fast_memset(taso3 + offset, red, 6);

    offset += 81 * STRIDE;

    RANGE_TASO0(taso0 + offset);
    fast_memset(taso0 + offset, 0, 6);
    fast_memset(taso1 + offset, 0, 6);
    fast_memset(taso2 + offset, red, 6);
    fast_memset(taso3 + offset, red, 6);
}

void piirra_kortti_bg(int offset) {
    int i;
    unsigned char *p;
    
    offset += 24 * STRIDE + 3;
    p = taso0 + offset;

    RANGE_TASO0(p);
    for (i = 0; i < 40; ++i) {
        fast_memset(p, 0xAA, 6); p += STRIDE;
        fast_memset(p, 0x55, 6); p += STRIDE;
    }
}

#define SHIFT2(a, b, n) do {                            \
        if (n > 0) {                                    \
            b |= (unsigned char)(a << (8 - n));         \
            a >>= n;                                    \
        }                                               \
    } while (0)

void draw_16x16_nshift(int offset, const unsigned char *p,
                        short x, short y, unsigned char red) {
    unsigned char *p0, *p1, *p2, *p3, c1, c2;
    int i, pstride = STRIDE - 2;
    offset += y * STRIDE + (x >> 3);

    p0 = taso0 + offset, p1 = taso1 + offset;
    p2 = taso2 + offset, p3 = taso3 + offset;

    for (i = 0; i < 16; ++i) {
        c1 = ~*p++, c2 = ~*p++;

        RANGE_TASO0(p0);
        *p0++ = *p0 & c1; *p0++ = *p0 & c2; p0 += pstride;
        *p1++ = *p1 & c1; *p1++ = *p1 & c2; p1 += pstride;
        if (red) continue;
        *p2++ = *p2 & c1; *p2++ = *p2 & c2; p2 += pstride;
        *p3++ = *p3 & c1; *p3++ = *p3 & c2; p3 += pstride;
    }
}

void draw_16x16_wshift(int offset, const unsigned char *p,
                        short x, short y, unsigned char red) {
    unsigned char *p0, *p1, *p2, *p3, c1, c2, c3;
    char b = x & 7, i;
    int pstride = STRIDE - 3;
    offset += y * STRIDE + (x >> 3);

    p0 = taso0 + offset, p1 = taso1 + offset;
    p2 = taso2 + offset, p3 = taso3 + offset;

    for (i = 0; i < 16; ++i) {
        c1 = *p++, c2 = *p++, c3 = 0;
        /* 3 tavun bittisiirto */
        SHIFT2(c2, c3, b);
        SHIFT2(c1, c2, b);
        c1 = ~c1, c2 = ~c2, c3 = ~c3;

        RANGE_TASO0(p0);
        *p0++ = *p0 & c1; *p0++ = *p0 & c2; *p0++ = *p0 & c3; p0 += pstride;
        *p1++ = *p1 & c1; *p1++ = *p1 & c2; *p1++ = *p1 & c3; p1 += pstride;
        
        if (red) continue;
        *p2++ = *p2 & c1; *p2++ = *p2 & c2; *p2++ = *p2 & c3; p2 += pstride;
        *p3++ = *p3 & c1; *p3++ = *p3 & c2; *p3++ = *p3 & c3; p3 += pstride;
    }
}

void draw_16x80_wshift_blue(int offset, const unsigned char *p,
                        short x, short y) {
    unsigned char *p1, *p2, c1, c2, c3;
    char b = x & 7, i;
    int pstride = STRIDE - 3;
    offset += y * STRIDE + (x >> 3);

    p1 = taso1 + offset, p2 = taso2 + offset;

    for (i = 0; i < 88; ++i) {
        c1 = *p++, c2 = *p++, c3 = 0;
        /* 3 tavun bittisiirto */
        SHIFT2(c2, c3, b);
        SHIFT2(c1, c2, b);
        c1 = ~c1, c2 = ~c2, c3 = ~c3;

        *p1++ = *p1 & c1; *p1++ = *p1 & c2; *p1++ = *p1 & c3; p1 += pstride;
        *p2++ = *p2 & c1; *p2++ = *p2 & c2; *p2++ = *p2 & c3; p2 += pstride;
    }
}

const unsigned char *isot_maat[] = {
    G_spata, G_shertta, G_sruutu, G_sristi
};
const unsigned char *isot_maat_U[] = {
    G_spata_U, G_shertta_U, G_sruutu_U, G_sristi_U
};
const unsigned char *pienet_maat[] = {
    G_ppata, G_phertta, G_pruutu, G_pristi
};
const unsigned char *pienet_maat_U[] = {
    G_ppata_U, G_phertta_U, G_pruutu_U, G_pristi_U
};
const unsigned char *arvot[] = {
    G_arvoA, G_arvo2, G_arvo3, G_arvo4, G_arvo5, G_arvo6,
    G_arvo7, G_arvo8, G_arvo9, G_arvoT, G_arvoJ, G_arvoQ, G_arvoK
};
const unsigned char *arvot_U[] = {
    G_arvoA_U, G_arvo2_U, G_arvo3_U, G_arvo4_U, G_arvo5_U, G_arvo6_U,
    G_arvo7_U, G_arvo8_U, G_arvo9_U, G_arvoT_U, G_arvoJ_U, G_arvoQ_U, G_arvoK_U
};

void piirra_kortti_feat(short x, short y, int offset, short a, short m) {
    const unsigned char *pm = pienet_maat[m];
    const unsigned char *um = pienet_maat_U[m];
    unsigned char red = maa_red[m];
    offset += 3 + 24 * STRIDE;
    switch (a)
    {
    case 0: /* A */
        draw_16x16_nshift(offset, pm, 16, 32, red);
        break;
    case 2: /* 3 */
        draw_16x16_nshift(offset, pm, 16, 32, red);
    case 1: /* 2 */
        draw_16x16_nshift(offset, pm, 16, 0, red);
        draw_16x16_nshift(offset, um, 16, 64, red);
        break;
    case 4: /* 5 */
        draw_16x16_nshift(offset, pm, 16, 32, red);
    case 3: /* 4 */
        draw_16x16_nshift(offset, pm, 0, 0, red);
        draw_16x16_nshift(offset, um, 0, 64, red);
        draw_16x16_nshift(offset, pm, 32, 0, red);
        draw_16x16_nshift(offset, um, 32, 64, red);
        break;
    case 7: /* 8 */
        draw_16x16_nshift(offset, um, 16, 48, red);
    case 6: /* 7 */
        draw_16x16_nshift(offset, pm, 16, 16, red);
    case 5: /* 6 */
        draw_16x16_nshift(offset, pm, 0, 0, red);
        draw_16x16_nshift(offset, pm, 0, 32, red);
        draw_16x16_nshift(offset, um, 0, 64, red);
        draw_16x16_nshift(offset, pm, 32, 0, red);
        draw_16x16_nshift(offset, pm, 32, 32, red);
        draw_16x16_nshift(offset, um, 32, 64, red);
        break;
    case 8: /* 9 */
        draw_16x16_nshift(offset, pm, 16, 32, red);
        draw_16x16_nshift(offset, pm, 0, 0, red);
        draw_16x16_nshift(offset, pm, 0, 21, red);
        draw_16x16_nshift(offset, um, 0, 43, red);
        draw_16x16_nshift(offset, um, 0, 64, red);
        draw_16x16_nshift(offset, pm, 32, 0, red);
        draw_16x16_nshift(offset, pm, 32, 21, red);
        draw_16x16_nshift(offset, um, 32, 43, red);
        draw_16x16_nshift(offset, um, 32, 64, red);
        break;
    case 9: /* 10 */
        draw_16x16_nshift(offset, pm, 16, 16, red);
        draw_16x16_nshift(offset, um, 16, 48, red);
        draw_16x16_nshift(offset, pm, 0, 0, red);
        draw_16x16_nshift(offset, pm, 0, 21, red);
        draw_16x16_nshift(offset, um, 0, 43, red);
        draw_16x16_nshift(offset, um, 0, 64, red);
        draw_16x16_nshift(offset, pm, 32, 0, red);
        draw_16x16_nshift(offset, pm, 32, 21, red);
        draw_16x16_nshift(offset, um, 32, 43, red);
        draw_16x16_nshift(offset, um, 32, 64, red);
        break;
    case 10: /* J */
        piirra_kuva(x + 24, y + 24, 48, 80, G_jatka);
        break;
    case 11: /* Q */
        piirra_kuva(x + 24, y + 24, 48, 80, G_akka);
        break;
    case 12: /* K */
        piirra_kuva(x + 24, y + 24, 48, 80, G_kurko);
        break;
    }
}

void piirra_kortti_corners(int offset, short a, short m) {
    const unsigned char *pm = isot_maat[m];
    const unsigned char *um = isot_maat_U[m];
    const unsigned char *pa = arvot[a];
    const unsigned char *ua = arvot_U[a];
    unsigned char red = maa_red[m];

    draw_16x16_wshift(offset, pa, 6, 10, red);
    draw_16x16_wshift(offset, pm, 6, 28, red);
    draw_16x16_wshift(offset, ua, 74, 102, red);
    draw_16x16_wshift(offset, um, 74, 84, red);
}

void piirra_kortti(short x, short y, kortti_t kortti) {
    char m = KMAA(kortti), a = KARVO(kortti);
    COMPUTE_COORD(x, y, offset, bitoffset);
    piirra_lakana(offset);

    if (a < 13) /* A, 2, 3, 4, 5, 6, 7, 8, 9, 10, J, Q, K */
    {
        piirra_kortti_frame(offset, maa_red[m]);
        if (a < 10)
            piirra_kortti_bg(offset);
        piirra_kortti_feat(x, y, offset, a, m);
        piirra_kortti_corners(offset, a, m);
    }
    else if (a == 15)
    {
        /* jokeri */
        draw_16x80_wshift_blue(offset, G_arvojok, 6, 18);
        piirra_kuva(x + 24, y + 24, 48, 80, G_jokeri);
    }
}

void pyyhi_kortti(short x, short y) {
    x &= ~7;
    piirra_suorakulmio2(x, y, KORTTI_L, KORTTI_K, 0);
}

void card_move(short x1, short y1, short x2, short y2,
                char a, char b, kortti_t c) {
    char close_l, close_r, close_u, close_d;
    x1 &= ~7; x2 &= ~7;
    close_l = x1 > x2 && x1 - x2 < KORTTI_L;
    close_r = x2 > x1 && x2 - x1 < KORTTI_L;
    close_u = y1 > y2 && y1 - y2 < KORTTI_K;
    close_d = y2 > y1 && y2 - y1 < KORTTI_K;
    if (x1 == x2 && y1 == y2)
        return;
    if ((!close_l && !close_r) || (!close_u && !close_d)) {
        pyyhi_kortti(x1, y1);
    } else {
        if (close_l)
            piirra_suorakulmio2(x2 + KORTTI_L, y1, x1 - x2, KORTTI_K, 0);
        else if (close_r)
            piirra_suorakulmio2(x1, y1, x2 - x1, KORTTI_K, 0);
        if (close_u)
            piirra_suorakulmio2(x1, y2 + KORTTI_K, KORTTI_L, y1 - y2, 0);
        else if (close_d)
            piirra_suorakulmio2(x1, y1, KORTTI_L, y2 - y1, 0);
        piirra_suorakulmio2(x2, y2, KORTTI_L, 8, 0);
        piirra_suorakulmio2(x2, y2 + KORTTI_K - 8, KORTTI_L, 8, 0);
    }
    if (!a)
        ;
    else if (b)
        piirra_kortti(x2, y2, c);
    else
        piirra_selka(x2, y2);
}

void siirra_tyhja(short x1, short y1, short x2, short y2) {
    card_move(x1, y1, x2, y2, 0, 0, 0);
}

void siirra_selka(short x1, short y1, short x2, short y2) {
    card_move(x1, y1, x2, y2, 1, 0, 0);
}

void siirra_kortti(short x1, short y1, short x2, short y2, kortti_t kortti) {
    card_move(x1, y1, x2, y2, 1, 1, kortti);
}
