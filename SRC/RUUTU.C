
#include <dos.h>
#include <stdlib.h>
#include <stddef.h>
#include "POKERI.H"
#include "VGA.H"
#include "RUUTU.H"
#include "VALOT.H"
#include "KORTTI.H"
#include "TEKSTI.H"
#include "KUVAT.H"
#include "PELI.H"
#include "POYTA.H"
#include "MUISTI.H"
#include "KENO.H"
#include <assert.h>
#if RECT_DUMP
#include <stdio.h>
#endif

typedef unsigned long intptr_t;

static unsigned char * const VGA_MEM = (unsigned char *)0xa0000;
static unsigned char * const VGA_TMEM = (unsigned char *)0xb8000;

/* 4 tasoa, 640*480 per taso */
static unsigned int total_frames = 0;

unsigned char *screen;
unsigned char *taso0, *taso1, *taso2, *taso3;
unsigned char *ylapalkki_tausta;
unsigned char *valittu_kuva;
unsigned char *tuplaus_viesti;
unsigned char *selka_cache;
unsigned char *kortti_cache;
unsigned char *kortti_cache1;
unsigned char *kortti_cache2;
unsigned char *kortti_cache3;
unsigned char *kortti_cache4;
unsigned char *old_vga_palette;

char _paivita_palkki = 1;
char _paivita_ylapalkki = 0;
char _paivita_alapalkki = 0;
char _paivita_ylapalkki_voitot = 0;
char _paivita_alapalkki_voitto = 0;
char _paivita_alapalkki_valinnat = 0;

#if RECT_DUMP
FILE* rectlog;
#endif

#define RGB(r, g, b) (r>>2),(g>>2),(b>>2)

static const unsigned char custom_palette[] = {
    RGB(  0,  0,  0), RGB(  0,  0,192), RGB(  0,128,  0), RGB(  0,128,128),
    RGB(160,  0,  0), RGB(160,  0,160), RGB(128, 96,  0), RGB(192,192,192),
    RGB(128,128,128), RGB(  0,  0,255), RGB(  0,255,  0), RGB(  0,255,255),
    RGB(255,  0,  0), RGB(255,  0,255), RGB(255,255,  0), RGB(255,255,255)
};

static const unsigned char custom_palette2[] = {
    RGB(  0,  0,  0), RGB(  0,  0,192), RGB(  0, 64,  0), RGB(  0, 64,128),
    RGB(160,  0,  0), RGB(160,  0,160), RGB(128, 96,  0), RGB(192, 96,192),
    RGB(128, 64,128), RGB(  0,  0,255), RGB(  0,128,  0), RGB(  0,128,255),
    RGB(255,  0,  0), RGB(255,  0,255), RGB(255,128,  0), RGB(255,128,255)
};

static const unsigned char custom_palette3[] = {
    RGB(  0,  0,  0), RGB(  0,  0, 48), RGB(  0,128,  0), RGB(  0,128, 32),
    RGB(160,  0,  0), RGB(160,  0, 40), RGB(128, 96,  0), RGB(192,192, 48),
    RGB(128,128, 32), RGB(  0,  0, 64), RGB(  0,255,  0), RGB(  0,255, 64),
    RGB(255,  0,  0), RGB(255,  0, 64), RGB(255,255,  0), RGB(255,255, 64)
};

static const unsigned char custom_palette4[] = {
    RGB(  0,  0,  0), RGB(  0,  0, 96), RGB(  0, 64,  0), RGB(  0, 64, 64),
    RGB(160,  0,  0), RGB(160,  0, 80), RGB(128, 48,  0), RGB(192, 96, 96),
    RGB(128, 64, 64), RGB(  0,  0,127), RGB(  0,127,  0), RGB(  0,127,127),
    RGB(255,  0,  0), RGB(255,  0,127), RGB(255,127,  0), RGB(255,127,127)
};

struct arena {
    unsigned char **ptr;
    unsigned int size;
};

struct rect {
    int o;
    int w;
    int h;
};

static struct rect urects[64];
short urects_n = 0;

#define IMAGE_SIZE(w, h) ((w) * (h) * 4 / (PPB))
static const struct arena arenas[] = {
    { &taso0,                   PLANE_SIZE }, 
    { &taso1,                   PLANE_SIZE },
    { &taso2,                   PLANE_SIZE },
    { &taso3,                   PLANE_SIZE },
    { &ylapalkki_tausta,        IMAGE_SIZE(GAREA_WIDTH, 32) },
    { &valittu_kuva,            IMAGE_SIZE(96, 32) },
    { &tuplaus_viesti,          IMAGE_SIZE(240, 16) },
    { &selka_cache,             IMAGE_SIZE(80, 112) },
    { &kortti_cache,            IMAGE_SIZE(KORTTI_L, KORTTI_K) },
    { &kortti_cache1,           IMAGE_SIZE(KORTTI_L, KORTTI_K) },
    { &kortti_cache2,           IMAGE_SIZE(KORTTI_L, KORTTI_K) },
    { &kortti_cache3,           IMAGE_SIZE(KORTTI_L, KORTTI_K) },
    { &kortti_cache4,           IMAGE_SIZE(KORTTI_L, KORTTI_K) },
    { &old_vga_palette,         192 }
};
#define ARENAS_N ((sizeof(arenas)) / (sizeof(arenas[0])))
int test = IMAGE_SIZE(24 * 12 + KORTTI_L, 56 * 3 + KORTTI_K);

void free_screen(void) {
#if RECT_DUMP
    fclose(rectlog);
#endif
    free(screen);
}

char alusta_ruutu(unsigned long *koko) {
    int i;
    unsigned long total_size = 4;
    unsigned char *buf;

    for (i = 0; i < ARENAS_N; ++i)
        total_size += arenas[i].size;

    *koko = total_size;
    screen = calloc(total_size, 1);
    if (!screen)
        return 1;
    atexit(free_screen);

    buf = (unsigned char*)(((intptr_t)screen + 3) & ~3); /* align to dword */
    for (i = 0; i < ARENAS_N; ++i) {
        *arenas[i].ptr = buf;
        buf += arenas[i].size;
    }
    vga_setup_copy();
#if RECT_DUMP
    rectlog = fopen("rect.log", "w");
    if (!rectlog) return 1;
#endif
    return 0;
}

static int palette_changed = 0;

static void vaihda_palettia(const unsigned char *palette) {
    vga_set_palette(palette, 0, 6);
    vga_set_palette(palette + 3 * 6, 20, 1);
    vga_set_palette(palette + 3 * 7, 7, 1);
    vga_set_palette(palette + 3 * 8, 56, 8);
    palette_changed = 1;
}

void vaihda_normaaliin_palettiin(void) {
    vaihda_palettia(custom_palette);
}

void vaihda_liilaan_palettiin(void) {
    vaihda_palettia(custom_palette2);
}

void vaihda_keltaiseen_palettiin(void) {
    vaihda_palettia(custom_palette3);
}

void vaihda_punaiseen_palettiin(void) {
    vaihda_palettia(custom_palette4);
}

short ruudun_vaihto(short tila, char oikeesti) {
    short vtila = switch_to_vga(tila);
    if (vtila >= 0 && oikeesti) {
        fast_memset(VGA_TMEM, 0, 0x8000);
        vga_get_palette(old_vga_palette, 64);
        vaihda_normaaliin_palettiin();
    }
    return vtila;
}

void ruudun_palautus(short tila) {
    if (palette_changed)
        vga_set_palette(old_vga_palette, 0, 64);
    restore_mode(tila);
}

void tyhjenna_ruutu(void) {
    fast_large_memset(taso0, 0, PLANE_SIZE);
    fast_large_memset(taso1, 0, PLANE_SIZE);
    fast_large_memset(taso2, 0, PLANE_SIZE);
    fast_large_memset(taso3, 0, PLANE_SIZE);
}

void piirra_tausta(void) {
    tyhjenna_ruutu();
}

const unsigned char COL[] = { 0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01 };

void piirra_suorakulmio(short x, short y, short w, short h, short vari) {
    int offset, pstride;
    short i;
    short x1b = x >> 3, x2b = (x + w) >> 3;
    short x1o = x  & 7, x2o = (x + w)  & 7;
    unsigned char l = COL[x1o], r = ~COL[x2o];
    unsigned char *p0, *p1, *p2, *p3;
    unsigned char m0 = ((vari     ) & 1) ? 0xFF : 0x00;
    unsigned char m1 = ((vari >> 1) & 1) ? 0xFF : 0x00;
    unsigned char m2 = ((vari >> 2) & 1) ? 0xFF : 0x00;
    unsigned char m3 = ((vari >> 3) & 1) ? 0xFF : 0x00;

    offset = y * STRIDE + x1b;
    p0 = taso0 + offset, p1 = taso1 + offset,
    p2 = taso2 + offset, p3 = taso3 + offset;

    if (!x1o && !x2o) {
        int w = x2b - x1b;
        for (i = 0; i < h; ++i) {
            fast_memset(p0, m0, w); p0 += STRIDE;
            fast_memset(p1, m1, w); p1 += STRIDE;
            fast_memset(p2, m2, w); p2 += STRIDE;
            fast_memset(p3, m3, w); p3 += STRIDE;
        }
    } else if (x1b == x2b) {
        l &= r;
        pstride = STRIDE - 1;
        for (i = 0; i < h; ++i) {
            *p0++ = (~l & *p0) | (l & m0); p0 += pstride;
            *p1++ = (~l & *p1) | (l & m1); p1 += pstride;
            *p2++ = (~l & *p2) | (l & m2); p2 += pstride;
            *p3++ = (~l & *p3) | (l & m3); p3 += pstride;
        }
    } else if (x1b + 1 == x2b) {
        pstride = STRIDE - 2;
        for (i = 0; i < h; ++i) {
            *p0++ = (~l & *p0) | (l & m0);
            *p0++ = (~r & *p0) | (r & m0); p0 += pstride;
            *p1++ = (~l & *p1) | (l & m1);
            *p1++ = (~r & *p1) | (r & m1); p1 += pstride;
            *p2++ = (~l & *p2) | (l & m2);
            *p2++ = (~r & *p2) | (r & m2); p2 += pstride;
            *p3++ = (~l & *p3) | (l & m3);
            *p3++ = (~r & *p3) | (r & m3); p3 += pstride;
        }
    } else {
        int w = x2b - x1b - 1;
        pstride = STRIDE - 2 - w;
        for (i = 0; i < h; ++i) {
            *p0++ = (~l & *p0) | (l & m0); fast_memset(p0, m0, w); p0 += w;
            *p0++ = (~r & *p0) | (r & m0); p0 += pstride;
            *p1++ = (~l & *p1) | (l & m1); fast_memset(p1, m1, w); p1 += w;
            *p1++ = (~r & *p1) | (r & m1); p1 += pstride;
            *p2++ = (~l & *p2) | (l & m2); fast_memset(p2, m2, w); p2 += w;
            *p2++ = (~r & *p2) | (r & m2); p2 += pstride;
            *p3++ = (~l & *p3) | (l & m3); fast_memset(p3, m3, w); p3 += w;
            *p3++ = (~r & *p3) | (r & m3); p3 += pstride;
        }
    }
}

void piirra_suorakulmio2(short x, short y, short w, short h, short vari) {
    if (x < 0) w += x, x = 0;
    if (y < 0) h += y, y = 0;
    if (x + w > GAREA_WIDTH) w = GAREA_WIDTH - x;
    if (y + h > CARDAREA_HEIGHT) h = CARDAREA_HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    piirra_suorakulmio(x, y, w, h, vari);
}

void sailyta_kuva(short x, short y, short w, short h, unsigned char* kuva) {
    short i;
    int l, o;
    unsigned char *s0, *s1, *s2, *s3;
    const unsigned char *p0, *p1, *p2, *p3;
    
    assert((x & 7) == 0);
    assert((w & 7) == 0);
    x >>= 3, w >>= 3;
    l = w * h;
    o = y * STRIDE + x;
    s0 = kuva, s1 = kuva + l, s2 = kuva + 2 * l, s3 = kuva + 3 * l;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;

    for (i = 0; i < h; ++i) {
        fast_memcpy(s0, p0, w); s0 += w; p0 += STRIDE;
        fast_memcpy(s1, p1, w); s1 += w; p1 += STRIDE;
        fast_memcpy(s2, p2, w); s2 += w; p2 += STRIDE;
        fast_memcpy(s3, p3, w); s3 += w; p3 += STRIDE;
    }
}

void piirra_kuva_rajaa_y(short x, short y, short w, short h, short rh,
                const unsigned char* kuva, const short max_y) {
    int l, o;
    const unsigned char *s0, *s1, *s2, *s3;
    unsigned char *p0, *p1, *p2, *p3;
    
    assert(h > 0);
    assert((x & 7) == 0);
    assert((w & 7) == 0);
    x >>= 3, w >>= 3;
    l = w * rh;
    o = y * STRIDE + x;
    s0 = kuva, s1 = kuva + l, s2 = kuva + 2 * l, s3 = kuva + 3 * l;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;
    h += y;
    if (h > max_y) h = max_y;
    if (y < 0) {
        s0 -= y * w, s1 -= y * w, s2 -= y * w, s3 -= y * w;
        h += y;
        y = 0;
    }

    for (; y < h; ++y) {
        fast_memcpy(p0, s0, w); p0 += STRIDE; s0 += w;
        fast_memcpy(p1, s1, w); p1 += STRIDE; s1 += w;
        fast_memcpy(p2, s2, w); p2 += STRIDE; s2 += w;
        fast_memcpy(p3, s3, w); p3 += STRIDE; s3 += w;
    }
}

void piirra_kuva_rajaa(short tx, short ty, short tw, short th,
                       short sx, short sy, short sw, short sh,
                       const unsigned char* kuva) {
    int l, o;
    const unsigned char *s0, *s1, *s2, *s3;
    unsigned char *p0, *p1, *p2, *p3;
    
    assert(th > 0);
    assert((tx & 7) == 0);
    assert((tw & 7) == 0);
    assert((sx & 7) == 0);
    assert((sw & 7) == 0);
    tx >>= 3, tw >>= 3;
    sx >>= 3, sw >>= 3;
    l = sw * sh;
    o = ty * STRIDE + tx;
    kuva += sy * sw + sx;
    s0 = kuva, s1 = kuva + l, s2 = kuva + 2 * l, s3 = kuva + 3 * l;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;
    th += ty;

    for (; ty < th; ++ty) {
        fast_memcpy(p0, s0, tw); p0 += STRIDE; s0 += sw;
        fast_memcpy(p1, s1, tw); p1 += STRIDE; s1 += sw;
        fast_memcpy(p2, s2, tw); p2 += STRIDE; s2 += sw;
        fast_memcpy(p3, s3, tw); p3 += STRIDE; s3 += sw;
    }
}

void piirra_kuva(short x, short y, short w, short h,
                const unsigned char* kuva) {
    piirra_kuva_rajaa_y(x, y, w, h, h, kuva, PLANE_HEIGHT);
}

void piirra_kuva_kortti(short x, short y, short w, short h,
                    const unsigned char* kuva) {
    piirra_kuva_rajaa_y(x, y, w, h, h, kuva, CARDAREA_HEIGHT);
}

void piirra_kuva_maski(short x, short y, short w, short h,
                const unsigned char* kuva, const unsigned char* maski) {
    const unsigned char *s0, *s1, *s2, *s3, *m;
    unsigned char *p0, *p1, *p2, *p3, c;
    int o, s, pstride;
    assert((x & 7) == 0);
    assert((w & 7) == 0);
    x >>= 3;
    w = (w + 7) >> 3;
    o = y * STRIDE + x;
    s = w * h;
    pstride = STRIDE - w;

    s0 = kuva, s1 = kuva + s, s2 = kuva + 2 * s, s3 = kuva + 3 * s, m = maski;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;

    for (s = 0; s < h; ++s) {
        for (o = 0; o < w; ++o) {
            c = *m++;
            *p0++ = (*p0 & ~c) | (*s0++ & c);
            *p1++ = (*p1 & ~c) | (*s1++ & c);
            *p2++ = (*p2 & ~c) | (*s2++ & c);
            *p3++ = (*p3 & ~c) | (*s3++ & c);
        }
        p0 += pstride, p1 += pstride, p2 += pstride, p3 += pstride;
    }
}

static void piirra_kuva_maski2(short x, short y, short w, short h,
                short sx, short sy, short sw, short sh,
                const unsigned char* kuva, unsigned char maski,
                unsigned char transp) {
    const unsigned char *s0, *s1, *s2, *s3;
    unsigned char m, *p0, *p1, *p2, *p3;
    int o, s, pstride, sstride;
    assert((x & 7) == 0);
    assert((w & 7) == 0);
    assert((sx & 7) == 0);
    assert((sw & 7) == 0);

    x >>= 3, w >>= 3;
    o = y * STRIDE + x;
    sstride = sw >> 3;
    pstride = STRIDE - w;
    kuva += ((sy * sw + sx) >> 3);
    s = sstride * sh;
    sstride -= w;

    s0 = kuva, s1 = kuva + s, s2 = kuva + 2 * s, s3 = kuva + 3 * s;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;
    m = maski;

    for (s = 0; s < h; ++s) {
        for (o = 0; o < w; ++o) {
            if (transp)
                m = maski & (*s0 | *s1 | *s2 | *s3);
            *p0++ = (*p0 & ~m) | (*s0++ & m);
            *p1++ = (*p1 & ~m) | (*s1++ & m);
            *p2++ = (*p2 & ~m) | (*s2++ & m);
            *p3++ = (*p3 & ~m) | (*s3++ & m);
        }
        p0 += pstride, p1 += pstride, p2 += pstride, p3 += pstride;
        s0 += sstride, s1 += sstride, s2 += sstride, s3 += sstride;
    }
}

#define SHIFT_READ(p, s) ((unsigned char)(((((unsigned short)(p)[0]) << 8) | ((unsigned short)(p)[1])) >> (s)))

static void piirra_kuva_maski2s(short x, short y, short w, short h,
                short sx, short sy, short sw, short sh,
                const unsigned char* kuva, unsigned char maski,
                unsigned char transp, unsigned char shift) {
    const unsigned char *s0, *s1, *s2, *s3;
    unsigned char m, *p0, *p1, *p2, *p3, c0, c1, c2, c3;
    int o, s, pstride, sstride;
    assert((x & 7) == 0);
    assert((w & 7) == 0);
    assert((sx & 7) == 0);
    assert((sw & 7) == 0);
    assert(shift <= 8);
    shift = 8 - shift;

    x >>= 3, w >>= 3;
    o = y * STRIDE + x;
    sstride = sw >> 3;
    pstride = STRIDE - w;
    kuva += ((sy * sw + sx) >> 3);
    s = sstride * sh;
    sstride -= w;

    s0 = kuva, s1 = kuva + s, s2 = kuva + 2 * s, s3 = kuva + 3 * s;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;
    m = maski;

    for (s = 0; s < h; ++s) {
        for (o = 0; o < w; ++o) {
            c0 = SHIFT_READ(s0, shift);
            c1 = SHIFT_READ(s1, shift);
            c2 = SHIFT_READ(s2, shift);
            c3 = SHIFT_READ(s3, shift);
            if (transp)
                m = maski & (c0 | c1 | c2 | c3);
            *p0++ = (*p0 & ~m) | (c0 & m);
            *p1++ = (*p1 & ~m) | (c1 & m);
            *p2++ = (*p2 & ~m) | (c2 & m);
            *p3++ = (*p3 & ~m) | (c3 & m);
            s0++, s1++, s2++, s3++;
        }
        p0 += pstride, p1 += pstride, p2 += pstride, p3 += pstride;
        s0 += sstride, s1 += sstride, s2 += sstride, s3 += sstride;
    }
}

static const unsigned char maski_w[9] = {
            0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF };
#define SUIKALE_MASKI(x, w) (unsigned char)(maski_w[(w)] >> (x))

void piirra_kuva_suikale(short tx, short ty, short tw, short th,
                         short sx, short sy, short sw, short sh,
                         const unsigned char* kuva, unsigned char transp) {
    unsigned char sxd = sx & 7, txd = tx & 7;
    short sxo = sx & ~7, txo = tx & ~7;
    if (!tw) return;
    if (sxd == txd) {
        unsigned char ow = 8 - txd;
        piirra_kuva_maski2(txo, ty, 8, th, sxo, sy, sw, sh,
                        kuva, SUIKALE_MASKI(txd, tw >= 8 ? 8 : tw), transp);
        tw = tw > ow ? tw - ow : 0;
        sxo += 8, txo += 8, sxd = 0, txd = 0;
        if (tw >= 8) {
            unsigned short vw = tw & ~7;
            piirra_kuva_maski2(txo, ty, tw & ~7, th, sxo, sy, sw, sh,
                            kuva, 0xFF, transp);
            tw &= 7, txo += vw, sxo += vw;
        }
        if (tw)
            piirra_kuva_maski2(txo, ty, 8, th, sxo, sy, sw, sh,
                            kuva, SUIKALE_MASKI(txd, tw), transp);
    } else {
        unsigned char shift;
        unsigned char ow = 8 - txd;
        if (sxd >= txd)
            shift = sxd - txd;
        else {
            shift = 8 + sxd - txd;
            --kuva;
        }

        piirra_kuva_maski2s(txo, ty, 8, th, sxo, sy, sw, sh,
                        kuva, SUIKALE_MASKI(txd, tw >= 8 ? 8 : tw), transp, shift);
        tw = tw > ow ? tw - ow : 0;
        sxo += 8, txo += 8, sxd = 0, txd = 0;
        if (tw >= 8) {
            unsigned short vw = tw & ~7;
            piirra_kuva_maski2s(txo, ty, vw, th, sxo, sy, sw, sh,
                            kuva, 0xFF, transp, shift);
            tw &= 7, txo += vw, sxo += vw;
        }
        if (tw)
            piirra_kuva_maski2s(txo, ty, 8, th, sxo, sy, sw, sh,
                            kuva, SUIKALE_MASKI(txd, tw), transp, shift);
    }
}

void piirra_kuva_vierita(short x, short y, short w, short h,
                const unsigned char* kuva, short h1, short h2, short sy) {
    const unsigned char *s0, *s1, *s2, *s3, *sa, *sb;
    unsigned char *p0, *p1, *p2, *p3, c;
    int o, s, pstride;
    assert((x & 7) == 0);
    assert((w & 7) == 0);
    x >>= 3; w >>= 3;
    o = y * STRIDE + x;
    s = w * h;

    sa = s0 = kuva, sb = s1 = kuva + s, s2 = kuva + 2 * s, s3 = kuva + 3 * s;
    sy *= w;
    s0 -= sy, s1 -= sy, s2 -= sy, s3 -= sy;
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;

    for (s = 0; s < h; ++s) {
        if (y + s < h1) continue;
        if (y + s >= h2) break;
        if (s0 >= sb || s0 < sa) {
            fast_memset(p0, 0xFF, w);
            fast_memset(p1, 0xFF, w);
            fast_memset(p2, 0xFF, w);
            fast_memset(p3, 0, w);
        } else if (s0 >= sa) {
            fast_memcpy(p0, s0, w);
            fast_memcpy(p1, s1, w);
            fast_memcpy(p2, s2, w);
            fast_memcpy(p3, s3, w);
        }
        s0 += w, s1 += w, s2 += w, s3 += w;
        p0 += STRIDE, p1 += STRIDE, p2 += STRIDE, p3 += STRIDE;
    }
    paivita_alue(x, y, w, h);
}

static const unsigned char round_l[] = {
    0x03, 0x0F, 0x1F, 0x3F, 0x7F, 0x7F, 0xFF, 0xFF
};
static const unsigned char round_r[] = {
    0xC0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFE, 0xFF, 0xFF
};

void piirra_tuplaus_viesti(void) {
    int i, j, x, otv = tekstivali;
    unsigned char c;

    piirra_suorakulmio(0, 0, 240, 16, 5);
    for (i = 0; i < 16; ++i) {
        j = i & 8 ? i ^ 15 : i;
        c = round_l[j];
        x = 0 + i * STRIDE;
        taso0[x] |= c; taso1[x] |= c; taso2[x] |= c; taso3[x] |= c;
        x = 15 + i * STRIDE;
        taso0[x] |= c; taso1[x] |= c; taso2[x] |= c; taso3[x] |= c;
        c = round_r[j];
        x = 9 + i * STRIDE;
        taso0[x] |= c; taso1[x] |= c; taso2[x] |= c; taso3[x] |= c;
        x = 24 + i * STRIDE;
        taso0[x] |= c; taso1[x] |= c; taso2[x] |= c; taso3[x] |= c;
    }

    tekstivali = 3;
    piirra_suorakulmio(8, 0, 64, 16, 15);
    piirra_teksti(8, 2, 0, 1, "A23456", 0);
    
    tekstivali = otv;

    piirra_suorakulmio(128, 0, 64, 16, 15);
    piirra_teksti(128, 2, 0, 1, "8!9!#JQK", 0);
    
    if (english) {
        piirra_teksti(90, 2, 11, 1, "OR", 0);
        piirra_teksti(210, 2, 11, 1, "OR", 0);
    } else {
        piirra_teksti(88, 2, 11, 1, "TAI", 0);
        piirra_teksti(208, 2, 11, 1, "TAI", 0);
    }
    
    sailyta_kuva(0, 0, 240, 16, tuplaus_viesti);
}

void piirra_ylapalkki_tausta(void) {
    int S = 32 * STRIDE;
    fast_large_memset(taso0, 0xff, S);
    fast_large_memset(taso1, 0xff, S);
    fast_large_memset(taso2, 0xff, S);
    fast_large_memset(taso3, 0x00, S);

    piirra_suorakulmio(30, 3, 180, 24, 1);
    piirra_suorakulmio(31, 4, 178, 22, 7);
    piirra_suorakulmio(32, 5, 176, 20, 1);
    piirra_suorakulmio(33, 6, 174, 18, 7);
    piirra_suorakulmio(34, 7, 172, 16, 1);
    if (english) {
        piirra_teksti(36, 9, 7, 0, "CREDIT", 0);
    } else {
        piirra_teksti(36, 9, 7, 0, "PELIT", 0);
    }

    piirra_suorakulmio(430, 3, 180, 24, 1);
    piirra_suorakulmio(431, 4, 178, 22, 7);
    piirra_suorakulmio(432, 5, 176, 20, 1);
    piirra_suorakulmio(433, 6, 174, 18, 7);
    piirra_suorakulmio(434, 7, 172, 16, 1);
    if (english) {
        piirra_teksti(436, 9, 7, 0, "WINS", 0);
    } else {
        piirra_teksti(436, 9, 7, 0, "VOITOT", 0);
    }

    if (english) {
        piirra_teksti(280, 11, 1, 0, "BET", 0);
    } else {
        piirra_teksti(280, 11, 1, 0, "PANOS", 0);
    }
    piirra_kuva_maski(336, 4, 24, 26, G_lantti, G_lantti_M);
    
    sailyta_kuva(0, 0, GAREA_WIDTH, 32, ylapalkki_tausta);
}

void piirra_ylapalkki(void) {
    piirra_kuva(0, 0, GAREA_WIDTH, 32, ylapalkki_tausta);

    piirra_levea_luku_oikea(200, 9, 7, pelit, 0);
    piirra_levea_luku_oikea(600, 9, 7, voitot, 0);
    piirra_luku_keski(348, 9, 1, 0, panos, 0);

    paivita_alue(0, 0, GAREA_WIDTH, 32);
}

void piirra_ylapalkki_voitot(void) {
    piirra_suorakulmio(500, 7, 106, 16, 1);
    piirra_levea_luku_oikea(600, 9, 7, voitot, 0);
    paivita_alue(500, 7, 106, 16);
}

const unsigned char alapalkki_color[16] = {
    0xFF, 0xFE, 0xEE, 0xEA, 0xAA, 0xA8, 0x88, 0x80,
    0x00, 0x01, 0x11, 0x15, 0x55, 0x5D, 0xDD, 0xDF
};

void piirra_valittu_kuva() {
    piirra_suorakulmio(0, 0, 96, 32, 7);
    piirra_suorakulmio(5, 7, 86, 18, 11);
    piirra_suorakulmio(5, 7, 86, 18, 11);
    piirra_suorakulmio(6, 8, 84, 16, 7);
    piirra_suorakulmio(7, 9, 82, 14, 11);
    if (english) {
        piirra_teksti(30, 10, 1, 0, "HELD", 0);
    } else {
        piirra_teksti(15, 10, 1, 0, "VALITTU", 0);
    }
    sailyta_kuva(0, 0, 96, 32, valittu_kuva);
}

void piirra_selka_cache() {
    const int pw = 80 / PPB;
    const unsigned char *p = G_selka;
    unsigned char *p0 = taso0, *p1 = taso1, *p2 = taso2, *p3 = taso3;
    int i;

    RANGE_TASO0(p0);
    for (i = 0; i < 112; ++i) {
        fast_memcpy(p0, p, pw);      p0 += STRIDE;
        fast_memcpy(p1, p, pw);      p1 += STRIDE;
        fast_memset(p2, 0xff, pw);   p2 += STRIDE;
        fast_memset(p3, 0, pw);      p3 += STRIDE;
        p += pw;
    }

    sailyta_kuva(0, 0, 80, 112, selka_cache);
}

void piirra_alapalkki(void) {
    int o = ALAPALKKI_Y * STRIDE;
    fast_large_memset(taso0 + o, 0xff, 32 * STRIDE);
    fast_large_memset(taso1 + o, alapalkki_color[alapalkki_fade], 32 * STRIDE);
    fast_large_memset(taso2 + o, 0xff, 32 * STRIDE);
    fast_large_memset(taso3 + o, 0, 32 * STRIDE);
    if (alapalkki_fade == 0)
        piirra_valinnat(0);
    else if (alapalkki_fade == 8)
        keno ? keno_piirra_alapalkki_voitto(0) : piirra_alapalkki_voitto(0);
    paivita_alue(0, ALAPALKKI_Y, GAREA_WIDTH, 32);
}

void piirra_alapalkki_valinnat(void) {
    piirra_valinnat(1);
}

void piirra_palkki(void) {
    piirra_palkki_napit();
    paivita_alue(0, GAREA_HEIGHT + 8,
            PLANE_WIDTH, PLANE_HEIGHT - GAREA_HEIGHT - 8);
}


#define TUPLAUS_WIDTH 240

int tuplaus_banner_x = 0;
void paivita_tuplaus_alapalkki(void) {
    const int S = TUPLAUS_WIDTH / PPB;
    const int L = TUPLAUS_WIDTH / PPB * 16;
    const int y = ALAPALKKI_Y + 8;
    const unsigned char *p0, *p1, *p2, *p3, *p0a, *p0b;
    unsigned char *d0, *d1, *d2, *d3;
    unsigned short s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    int sx = tuplaus_banner_x, ox, oy;
    int xb = sx >> 3, xo = sx & 7, xs = 8 - xo;
    int i, j;

    for (i = 0; i < 16; ++i) {
        ox = xb + S * i;
        oy = ((y + i) * STRIDE + 416 / PPB) - 1;

        p0 = tuplaus_viesti + ox;
        p1 = tuplaus_viesti + ox + L;
        p2 = tuplaus_viesti + ox + L * 2;
        p3 = tuplaus_viesti + ox + L * 3;
        p0b = tuplaus_viesti + S * (i + 1);
        d0 = taso0 + oy, d1 = taso1 + oy, d2 = taso2 + oy, d3 = taso3 + oy;

        for (j = 0; j <= 16; ++j) {
            s0 <<= xs, s1 <<= xs, s2 <<= xs, s3 <<= xs;
            s0 |= *p0++; s1 |= *p1++; s2 |= *p2++; s3 |= *p3++;
            if (p0 == p0b) {
                p0 -= TUPLAUS_WIDTH / PPB;
                p1 -= TUPLAUS_WIDTH / PPB;
                p2 -= TUPLAUS_WIDTH / PPB;
                p3 -= TUPLAUS_WIDTH / PPB;
            }
            s0 <<= xo, s1 <<= xo, s2 <<= xo, s3 <<= xo;
            if (j) {
                *d0++ = (s0 >> 8);
                *d1++ = (s1 >> 8);
                *d2++ = (s2 >> 8);
                *d3++ = (s3 >> 8);
            } else {
                ++d0, ++d1, ++d2, ++d3;
            }
        }
    }

    tuplaus_banner_x = (tuplaus_banner_x + tickskip + 1) % TUPLAUS_WIDTH;
    paivita_alue(416, y, 128, 16);
}

void valmistele_ruutu(void) {
    piirra_ylapalkki_tausta();
    piirra_valittu_kuva();
    piirra_tuplaus_viesti();
    piirra_selka_cache();
    piirra_tausta();
    piirra_palkki_napit_eka();
}

void valmistele_ruutu_peli(void) {
    fast_memset(taso0, 0, GAREA_SIZE);
    fast_memset(taso1, 0, GAREA_SIZE);
    fast_memset(taso2, 0, GAREA_SIZE);
    fast_memset(taso3, 0, GAREA_SIZE);
    paivita_ylapalkki();
    paivita_alapalkki();
    paivita_palkki();
    paivita_alue(0, 0, PLANE_WIDTH, PLANE_HEIGHT);
}

void valmistele_ruutu_esittely(void) {
    fast_memset(taso0, 0, GAREA_SIZE);
    fast_memset(taso1, 0, GAREA_SIZE);
    fast_memset(taso2, 0, GAREA_SIZE);
    fast_memset(taso3, 0, GAREA_SIZE);
    paivita_alue(0, 0, GAREA_WIDTH, GAREA_HEIGHT);
}

void ruudun_paivitys(void) {
    if (_paivita_ylapalkki)
        piirra_ylapalkki(), _paivita_ylapalkki = 0;
    else if (_paivita_ylapalkki_voitot)
        piirra_ylapalkki_voitot(), _paivita_ylapalkki_voitot = 0;
    if (_paivita_alapalkki)
        piirra_alapalkki(), _paivita_alapalkki = 0;
    else if (_paivita_alapalkki_voitto)
        keno ? keno_piirra_alapalkki_vain_voitto(1)
             : piirra_alapalkki_vain_voitto(1), _paivita_alapalkki_voitto = 0;
    else if (_paivita_alapalkki_valinnat)
        keno ? (void)0
             : piirra_alapalkki_valinnat(), _paivita_alapalkki_valinnat = 0;
}

static void blit_all_rects(void);

void paivita_alue(unsigned short x, unsigned short y,
            unsigned short w, unsigned short h) {
    struct rect *rect;
    assert(w > 0 && h > 0);
    if (urects_n == sizeof(urects) / sizeof(urects[0])) {
        blit_all_rects();
        urects_n = 0;
    }
    rect = &urects[urects_n++];
    w = (w + 7) >> 3;
    if (x & 15)
        ++w;
    x = (x >> 3) & ~1;
    rect->o = y * STRIDE + x;
    rect->w = w, rect->h = h;
}

void blit_rects(unsigned char *src, struct rect *rct, unsigned n);

#pragma aux blit_rects = \
    "sub esp, 12" \
    "nextrect:" \
    "mov [esp+8], cx" \
    "mov ebx, [esi]" \
    "mov cx, [esi+4]" \
    "mov dx, [esi+8]" \
    "add esi, 12" \
    "mov [esp+4], edi" \
    "mov [esp], esi" \
    "lea esi, [ebx+edi]" \
    "lea edi, [ebx+0a0000h]" \
    \
    "inc ecx" \
    "mov eax, 80" /* STRIDE */ \
    "shr ecx, 1" \
    "sub eax, ecx" \
    "sub eax, ecx" \
    "mov ebx, ecx" \
    "nextrow:" \
    "rep movsw" \
    "add esi, eax" \
    "add edi, eax" \
    "mov cx, bx" \
    "dec dx" \
    "jnz nextrow" \
    \
    "mov esi, [esp]" \
    "mov edi, [esp+4]" \
    "mov cx, [esp+8]" \
    "loop nextrect" \
    "add esp, 12" \
    parm [edi] [esi] [ecx] modify exact [edi esi ecx edx eax ebx]

static void blit_all_rects(void) {
#if RECT_DUMP
    short i;
    fprintf(rectlog, "Frame #%u (Game = %s)\n", total_frames,
            pelitila_nimi(tila));
    for (i = 0; i < urects_n; ++i) {
        fprintf(rectlog, "  Rect #%u: (%u,%u,%u,%u)\n", i,
            (urects[i].o % STRIDE) << 3, urects[i].o / STRIDE,
            urects[i].w << 3, urects[i].h);
    }
#endif
    vga_select_planes(0x01); blit_rects(taso0, urects, urects_n);
    vga_select_planes(0x02); blit_rects(taso1, urects, urects_n);
    vga_select_planes(0x04); blit_rects(taso2, urects, urects_n);
    vga_select_planes(0x08); blit_rects(taso3, urects, urects_n);
}

void ruudun_piirto(void) {
    if (_paivita_palkki)
        piirra_palkki(), _paivita_palkki = 0;

    if (urects_n) {
        blit_all_rects();
        urects_n = 0;
    }
    ++total_frames;
}

void tahti(void) {
    vga_vsync();
}
