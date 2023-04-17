
#include "POKERI.H"
#include "TEKSTI.H"
#include "KUVAT.H"
#include "RUUTU.H"
#include "KORTTI.H"

int tekstivali = 2;

unsigned char nbuf[8] = { 0 };

int last_color = -1;
unsigned char taso0maski = 0;
unsigned char taso1maski = 0;
unsigned char taso2maski = 0;
unsigned char taso3maski = 0;

int last_bgcolor = -1;
unsigned char taso0maskibg = 0;
unsigned char taso1maskibg = 0;
unsigned char taso2maskibg = 0;
unsigned char taso3maskibg = 0;

const int vfont_width[] = {
    14,14,14,14,14,14,14,14,14,14,0,0,0,0,0,0,
    4,0,0,10,0,0,0,0,0,0,0,0,0,8,0,0,
    7,3,7,7,6,7,7,6,7,7,0,0,0,0,0,0,
    13,8,9,8,9,7,7,6,8,3,7,7,6,8,7,7,
    7,7,7,8,8,8,9,8,9,9,9,0,0,0,0,0,
    0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,6
};

unsigned char *render_wide_n(unsigned int n) {
    unsigned char *ns = &nbuf[6];
    for (;;) {
        *ns = 0x10 + (n % 10);
        if (n < 10)
            break;
        --ns;
        n /= 10;
    }
    return ns;
}

unsigned char *render_normal_n(unsigned int n) {
    unsigned char *ns = &nbuf[6];
    for (;;) {
        *ns = '0' + (n % 10);
        if (n < 10)
            break;
        --ns;
        n /= 10;
    }
    return ns;
}

#define UPDATE_FG_COLOR(color)  do {                                \
        if (color != last_color) {                                  \
            last_color = color;                                     \
            taso0maski = ((color     ) & 1) ? 0xff : 0x00;          \
            taso1maski = ((color >> 1) & 1) ? 0xff : 0x00;          \
            taso2maski = ((color >> 2) & 1) ? 0xff : 0x00;          \
            taso3maski = ((color >> 3) & 1) ? 0xff : 0x00;          \
        }                                                           \
    } while (0)

#define UPDATE_BG_COLOR(color)  do {                                \
        if (color != last_bgcolor) {                                \
            last_bgcolor = color;                                   \
            taso0maskibg = ((color     ) & 1) ? 0xff : 0x00;        \
            taso1maskibg = ((color >> 1) & 1) ? 0xff : 0x00;        \
            taso2maskibg = ((color >> 2) & 1) ? 0xff : 0x00;        \
            taso3maskibg = ((color >> 3) & 1) ? 0xff : 0x00;        \
        }                                                           \
    } while (0)

#define SHIFT2(a, b, n) do {                            \
        if (n > 0) {                                    \
            b |= (unsigned char)(a << (8 - n));         \
            a >>= n;                                    \
        }                                               \
    } while (0)

void draw_vfont_char(int x, int y, int color, unsigned char ch) {
    int i, o, b;
    const unsigned char *src;
    unsigned char *p0, *p1, *p2, *p3;
    unsigned char c1, c2, c3;
    unsigned short s;
    const unsigned int pstride = STRIDE - 3;

    UPDATE_FG_COLOR(color);

    src = G_vfont + (ch - 0x10) * 0x18;

    COMPUTE_COORD(x, y, o, b);
    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;

    for (i = 0; i < 12; ++i) {
        c1 = *src++, c2 = *src++, c3 = 0;
        /* 3 tavun bittisiirto */
        SHIFT2(c2, c3, b);
        SHIFT2(c1, c2, b);

        RANGE_TASO0(p0);
        *p0++ = (*p0 & ~c1) | (taso0maski & c1);
        *p0++ = (*p0 & ~c2) | (taso0maski & c2);
        *p0++ = (*p0 & ~c3) | (taso0maski & c3);
        p0 += pstride;

        *p1++ = (*p1 & ~c1) | (taso1maski & c1);
        *p1++ = (*p1 & ~c2) | (taso1maski & c2);
        *p1++ = (*p1 & ~c3) | (taso1maski & c3);
        p1 += pstride;

        *p2++ = (*p2 & ~c1) | (taso2maski & c1);
        *p2++ = (*p2 & ~c2) | (taso2maski & c2);
        *p2++ = (*p2 & ~c3) | (taso2maski & c3);
        p2 += pstride;

        *p3++ = (*p3 & ~c1) | (taso3maski & c1);
        *p3++ = (*p3 & ~c2) | (taso3maski & c2);
        *p3++ = (*p3 & ~c3) | (taso3maski & c3);
        p3 += pstride;
    }
}

void piirra_levea_luku(int x, int y, int vari,
                    unsigned int n, int paivita) {
    unsigned char *ns = render_wide_n(n);
    piirra_teksti(x, y, vari, 0, ns, paivita);
}

void piirra_levea_luku_oikea(int x, int y, int vari,
                    unsigned int n, int paivita) {
    unsigned char *ns = render_wide_n(n);
    piirra_teksti_oikea(x, y, vari, 0, ns, paivita);
}

int measure_vfont(const unsigned char *t, int lihavoi) {
    unsigned char c;
    int w = *t ? -tekstivali : 0;
    while (c = *t++) {
        if (c == 'Ä') c = 'a';
        if (c == 'Ö') c = 'o';
        if (c == '!') {
            ++w;
            continue;
        }
        if (c < 0x10 || c >= 0x6f) continue;
        w += vfont_width[c - 0x10] + lihavoi + tekstivali;
    }
    return w;
}

void piirra_teksti(int x, int y, int vari,
                    int lihavoi, const char *t, int paivita) {
    unsigned char c;
    int w, ox = x;
    while (c = *t++) {
        if (c == 'Ä') c = 'a';
        if (c == 'Ö') c = 'o';
        if (c == '!') {
            ++x;
            continue;
        }
        if (c < 0x10 || c >= 0x6f) continue;
        w = vfont_width[c - 0x10] + tekstivali;
        draw_vfont_char(x, y, vari, c);
        if (lihavoi)
            draw_vfont_char(++x, y, vari, c);
        x += w;
    }
    if (paivita)
        paivita_alue(ox, y, x - ox, 12);
}

void piirra_teksti_oikea(int x, int y, int vari,
                    int lihavoi, const char *t, int paivita) {
    piirra_teksti(x - measure_vfont(t, lihavoi), y,
            vari, lihavoi, t, paivita);
}

void piirra_teksti_keski(int x, int y, int vari,
                    int lihavoi, const char *t, int paivita) {
    piirra_teksti(x - (measure_vfont(t, lihavoi) + 1) / 2, y,
            vari, lihavoi, t, paivita);
}

void piirra_luku(int x, int y, int vari,
                    int lihavoi, unsigned int n, int paivita) {
    unsigned char *ns = render_normal_n(n);
    piirra_teksti_oikea(x, y, vari, lihavoi, ns, paivita);
}

void piirra_luku_oikea(int x, int y, int vari,
                    int lihavoi, unsigned int n, int paivita) {
    unsigned char *ns = render_normal_n(n);
    piirra_teksti_oikea(x, y, vari, lihavoi, ns, paivita);
}

void piirra_luku_keski(int x, int y, int vari,
                    int lihavoi, unsigned int n, int paivita) {
    unsigned char *ns = render_normal_n(n);
    piirra_teksti_keski(x, y, vari, lihavoi, ns, paivita);
}

void piirra_nappiteksti(int x, int y, const char *t, int vari) {
    int i, o, fg, bg, ox = x;
    const unsigned char *src;
    unsigned char *p0, *p1, *p2, *p3;
    unsigned char c, ch;
    unsigned short s;

    fg = vari & 15;
    bg = (vari >> 4) & 15;
    x *= 8;
    y *= 8;

    UPDATE_FG_COLOR(fg);
    UPDATE_BG_COLOR(bg);

    while (ch = *t++) {
        src = G_font + ch * 0x08;

        COMPUTE_COORD(x, y, o, i);
        p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;

        RANGE_TASO0(p0);
        for (i = 0; i < 8; ++i) {
            c = *src++;
            *p0 = (taso0maskibg & ~c) | (taso0maski & c); p0 += STRIDE;
            *p1 = (taso1maskibg & ~c) | (taso1maski & c); p1 += STRIDE;
            *p2 = (taso2maskibg & ~c) | (taso2maski & c); p2 += STRIDE;
            *p3 = (taso3maskibg & ~c) | (taso3maski & c); p3 += STRIDE;
        }

        x += 8;
    }
}
