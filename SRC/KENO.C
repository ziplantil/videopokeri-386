
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "POKERI.H"
#include "KENO.H"
#include "NAPIT.H"
#include "PELI.H"
#include "KORTTI.H"
#include "VALOT.H"
#include "RUUTU.H"
#include "NAPIT.H"
#include "PAKKA.H"
#include "POYTA.H"
#include "KUVAT.H"
#include "TEKSTI.H"
#include "SOITTO.H"
#include "AIKA.H"
#include "MUISTI.H"
#include <assert.h>

enum PelitilaKeno kenotila;
static int voitto;
static int voitto_disp = 0;
static int voitto_hitaus = 0;
static int voitto_vilkku = 0;
static int inactive = 0;
static int oli_paavoitto = 0;
static char palswap = 0;
static char new_mode = 0;
static int valintoja = 0;
static int osumat = 0;
int keno_valintoja_max = 0;
const int *keno_kertoimet_kaikki;
static const int *keno_kertoimet;
static char musan_toisto = 0;
static char skip_input_read = 0;
static char tickskip_i = 0;
static unsigned short siirto_kortit = 0;
static char valinta_x = 6;
static char valinta_y = 1;
static char valinta_vilkku = 0;
static unsigned short valinta_vilkku_anim = 0;
static unsigned short kortit_x[5];
static unsigned short kortit_y[5];
static short pakka_x;
static char tuplaus;
static char jaettu = 0;
static signed char jakoaika = 0;
static long ufrac = 0;
static char tuplaus_ctr = 0;
static int koputus_anim;

static char valitut_kortit[5];
static char osumakortit[5];

#define napit napit_kaikki.p.keno
#define valot valot_eli.keno

#define ANIM_EQ(n) ((anim >= (n)) && (anim <= ((n) + tickskip)))
#define SPEED_MUL(x) ((x) * (tickskip + 1))
#define PYSAYTA_AANET() (musan_toisto = 0, pysayta_aanet())

#define VALINTA_X 16
#define VALINTA_Y 48
#define VALINTA_KW 24
#define VALINTA_KH 56
#define PAKKA_X 528
#define PAKKA_Y 224
#define TUPLAUS_X 272
#define JAKO_L 40
#define JAKO_KORTTEJA 10
#define OSUMA_L 64
#define OSUMA_K 64

#define VALINTA_XY VALINTA_X + valinta_x * VALINTA_KW, \
                   VALINTA_Y + valinta_y * VALINTA_KH
#define VALINTA_KORTTI ((valinta_y << 4) + valinta_x)
#define RAITATILA_L (KORTTI_L + (ARVOT_N - 1) * VALINTA_KW)
#define RAITATILA_K (KORTTI_K + 3 * VALINTA_KH)
#define RAITA_L 1
#define RAITA_K (RAITATILA_K >> 2)
#define RAITA_LKM (RAITATILA_L * 4)

static unsigned short alut_x[6] = { 0, 176, 112, 64, 24, 16 };
static unsigned short valit_x[6] = {
    0,KORTTI_L + 48, KORTTI_L + 48, KORTTI_L + 24, KORTTI_L + 8, KORTTI_L - 8 };

static unsigned char *kortti_caches[6] = { 0 };
static unsigned char *raitatila = NULL;

/** "X   X   XXX   XXXX   xxxxx  xxxxx  X"
    "X  X   X   X  X   X    x      x    X"
    "XxX    X   X  XXXX     x      x    X"
    "X  X   X   X  X   x    x      x    X"
    "X   X   XXX   X   x    x      x    X"
    "                                    "
    "                                    "
    "     X   X  XXXXX  X   x   XXX      "
    "     X  X   X      Xx  X  X   X     "
    "     XxX    XXXXX  X x X  X   X     "
    "     X  X   X      X  xX  X   X     "
    "     X   X  XXXXX  X   x   XXX      "; */
static const unsigned char logokartta[] = {
    0x88, 0xe3, 0xc7, 0xcf, 0x90,
    0x91, 0x12, 0x21, 0x02, 0x10,
    0xe1, 0x13, 0xc1, 0x02, 0x10,
    0x91, 0x12, 0x21, 0x02, 0x10,
    0x88, 0xe2, 0x21, 0x02, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x04, 0x4f, 0x91, 0x1c, 0x00,
    0x04, 0x88, 0x19, 0x22, 0x00,
    0x07, 0x0f, 0x95, 0x22, 0x00,
    0x04, 0x88, 0x13, 0x22, 0x00,
    0x04, 0x4f, 0x91, 0x1c, 0x00,
};

void keno_piirra_alapalkki_vain_voitto(char paivita) {
    piirra_suorakulmio(264, ALAPALKKI_Y + 8, 112, 16, 1);
    piirra_levea_luku_oikea(344, ALAPALKKI_Y + 10, 15, voitto_disp, 0);
    if (paivita)
        paivita_alue(264, ALAPALKKI_Y + 8, 112, 16);
}

void keno_piirra_alapalkki_voitto(char paivita) {
    const int y = ALAPALKKI_Y;
    switch (kenotila)
    {
    case TK_VOITTO:
    case TK_TUPLAOK:
        if (english)
            piirra_teksti_oikea(160, y + 10, 11, 0, "YOU WON", 0);
        else
            piirra_teksti(160, y + 10, 11, 0, "VOITIT", 0);
        piirra_teksti(424, y + 10, 11, 0,
                    english ? "DOUBLE" : "TUPLAATKO", 0);
        piirra_kuva_maski(256, y + 8, 8, 16, G_tulosv, G_tulosv_M);
        piirra_kuva_maski(376, y + 8, 8, 16, G_tuloso, G_tuloso_M);
        break;
    case TK_TUPLA1:
    case TK_TUPLA2:
    case TK_TUPLA3:
    case TK_TUPLAEI:
    case TK_TUPLAUS:
        piirra_teksti(424, y + 10, 11, 0,
                english ? "DOUBLE" : "TUPLAATKO", 0);
    case TK_TUPLA4:
        piirra_teksti(160, y + 10, 11, 0, english ? "BET" : "PANOS", 0);
        piirra_kuva_maski(256, y + 8, 8, 16, G_tulosv, G_tulosv_M);
        piirra_kuva_maski(376, y + 8, 8, 16, G_tuloso, G_tuloso_M);
        break;
    }
    keno_piirra_alapalkki_vain_voitto(0);
    if (paivita)
        paivita_alue(0, y, GAREA_WIDTH, 32);
}

static void update_voitto_hitaus(int offset, char slow) {
    offset = SPEED_MUL(offset);
    if (offset <= 10)
        voitto_hitaus = 8;
    else if (offset <= 20)
        voitto_hitaus = 4;
    else
        voitto_hitaus = 2;
    if (slow) voitto_hitaus *= 2;
}

#define KERROIN_Y 48

static void piirra_keno_kertoimet(char hohda_aina) {
    short i, hohda = hohda_aina || (anim & 32),
          oy = KERROIN_Y, y, vari, kerroin;
    piirra_suorakulmio(512, KERROIN_Y, 120,
                       (keno_valintoja_max + 2) * 12, 0);
    y = oy + 2 * 12;
    if (valintoja) {
        short ny = y + keno_valintoja_max * 12;
        piirra_teksti(520, KERROIN_Y, 14, 0, english ? "match" : "osumat", 0);
        piirra_teksti(530, KERROIN_Y + 12, 14, 0, english ? " x" : "kpl", 0);
        piirra_teksti(580, KERROIN_Y, 14, 0, english ? "  win" : "voitot", 0);
        piirra_teksti(594, KERROIN_Y + 12, 14, 0, "mk", 0);
        if (!hohda && osumat > 0 && voitto)
            piirra_suorakulmio(512, y + 12 * (valintoja - osumat), 120, 12, 2);
        for (i = 0; i < valintoja; ++i) {
            kerroin = keno_kertoimet[i];
            if (!kerroin) break;
            vari = (hohda && valintoja - osumat == i) ? 4 : 14;
            piirra_luku_oikea(544, y, vari, 0, valintoja - i, 0);
            piirra_luku_oikea(608, y, vari, 0, panos * kerroin, 0);
            y += 12;
        }
        piirra_suorakulmio(512, y, 120, ny - y, 0);
        y = ny;
    } else {
        piirra_teksti(english ? 520 : 528, KERROIN_Y, 14, 0,
                english ? "please select" : "voit valita", 0);
        piirra_luku_oikea(530, KERROIN_Y + 12, 14, 0, 1, 0);
        piirra_teksti(534, KERROIN_Y + 12, 14, 0, "-", 0);
        piirra_luku(552, KERROIN_Y + 12, 14, 0, keno_valintoja_max, 0);
        piirra_teksti(560, KERROIN_Y + 12, 14, 0,
                english ? "cards" : "korttia", 0);
        y += keno_valintoja_max * 12;
    }
    paivita_alue(512, KERROIN_Y, 120, y - oy);
}

static void piirra_keno_kertoimet_uudelleen(void) {
    short i, hohda = anim & 32, y = KERROIN_Y + 2 * 12, vari, kerroin;
    if (!valintoja || !osumat) return;
    i = valintoja - osumat;
    y += 12 * i;
    piirra_suorakulmio(240, y, 352, 12, !hohda ? 2 : 0);
    vari = hohda ? 4 : 14;
    kerroin = keno_kertoimet[i];
    if (!kerroin) return;
    piirra_luku(576, y, vari, 0, valintoja - i, 0);
    piirra_luku_oikea(632, y, vari, 0, panos * kerroin, 0);
    paivita_alue(576, y, 64, 12);
}

static void vilkuta_voittokerrointa(void) {
    short i, j, y;
    int o, pstride = STRIDE - 120 / 8;
    unsigned char *p1, *p2, *p3;
    if (!valintoja || !osumat) return;
    i = valintoja - osumat;
    if (!keno_kertoimet[i]) return;
    y = KERROIN_Y + 2 * 12 + i * 12;
    o = y * STRIDE + 512 / 8;
    p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;
    for (i = 0; i < 12; ++i) {
        for (j = 0; j < 120 / 8; ++j) {
            *p3++ = (*p3 ^ 0xFF) & *p2++;
            *p1++ = *p1 ^ 0xFF;
        }
        p1 += pstride, p2 += pstride, p3 += pstride;
    }
    paivita_alue(512, y, 120, 12);
}

static void lopeta_esittely(void) {
    char i;
    PYSAYTA_AANET();
    valmistele_ruutu_peli();
    keno_alusta_tila(raitatila ? TK_SIIRTO0B : TK_SIIRTO0A);
    if (!napit.panos && !napit.jako)
        toista_aani(AANI_ALOITA);
    skip_input_read = 1;
}

// onko 8 pikselin rivi kortin kuva-alueella?
static int kuva_alueella(int y, int x) {
    return 3 <= x && x <= 8 && 24 <= y && y <= 103;
}

// maalaa kortin siniseksi
static void keno_valinta_korttiin(int x, int y, int lx, int ly) {
    int offset, pstride;
    unsigned char *p0, *p1, *p2, *p3;
    int yoffset, xoffset, xwidth;
    unsigned char a = 0x55;
    int kw = lx ? KORTTI_L : VALINTA_KW;
    int kh = ly ? KORTTI_K : VALINTA_KH;

    assert((x & 7) == 0);
    assert((kw & 7) == 0);

    kw >>= 3;
    pstride = STRIDE - kw;
    offset = y * STRIDE + (x >> 3);
    p0 = taso0 + offset, p1 = taso1 + offset,
    p2 = taso2 + offset, p3 = taso3 + offset;

    for (yoffset = 0; yoffset < kh; ++yoffset) {
        for (xoffset = 0; xoffset < kw; ++xoffset) {
            unsigned char w = *p0 & *p1 & *p2 & *p3;
            if (kuva_alueella(yoffset, xoffset))
                p0++, p1++, p2++, p3++;
            else
                p0++, p1++, *p2++ &= ~(w & a), p3++;
        }
        p0 += pstride, p1 += pstride, p2 += pstride, p3 += pstride;
        a ^= 0xFF;
    }
    paivita_alue(x, y, kw << 3, kh);
}

// maalaa sinisen kortin valkoiseksi
static void keno_valinta_pois_kortista(int x, int y, int lx, int ly) {
    int offset, pstride;
    unsigned char *p0, *p1, *p2, *p3;
    int yoffset, xoffset, xwidth;
    unsigned char a = 0x55;
    int kw = lx ? KORTTI_L : VALINTA_KW;
    int kh = ly ? KORTTI_K : VALINTA_KH;

    assert((x & 7) == 0);
    assert((kw & 7) == 0);

    kw >>= 3;
    pstride = STRIDE - kw;
    offset = y * STRIDE + (x >> 3);
    p0 = taso0 + offset, p1 = taso1 + offset,
    p2 = taso2 + offset, p3 = taso3 + offset;

    for (yoffset = 0; yoffset < kh; ++yoffset) {
        for (xoffset = 0; xoffset < kw; ++xoffset) {
            unsigned char w = *p0 & *p1 & ~*p2 & *p3;
            if (kuva_alueella(yoffset, xoffset))
                p0++, p1++, p2++, p3++;
            else
                p0++, p1++, *p2++ |= w, p3++;
        }
        p0 += pstride, p1 += pstride, p2 += pstride, p3 += pstride;
        a ^= 0xFF;
    }
    paivita_alue(x, y, kw << 3, kh);
}

static void keno_valitse_kortti(void) {
    kortti_t k = VALINTA_KORTTI;
    int i;
    for (i = 0; i < valintoja; ++i) {
        if (valitut_kortit[i] == k) {
            keno_valinta_pois_kortista(VALINTA_XY, valinta_x == ARVOT_N - 1,
                                       valinta_y == 3);
            for (; i < valintoja - 1; ++i)
                valitut_kortit[i] = valitut_kortit[i + 1];
            if (--valintoja == 0) {
                VALO_POIS(valot.jako);
                VALO_POIS(valot.peru);
            } else
                keno_kertoimet = &keno_kertoimet_kaikki[(valintoja - 1) * 5];
            toista_aani(AANI_VALINTA);
            piirra_keno_kertoimet(0);
            return;
        }
    }
    if (valintoja >= keno_valintoja_max)
        return;
    valitut_kortit[valintoja++] = k;
    VALO_VILKKU(valot.jako, 1);
    VALO_VILKKU(valot.peru, 1);
    toista_aani(AANI_VALINTA);
    keno_kertoimet = &keno_kertoimet_kaikki[(valintoja - 1) * 5];
    piirra_keno_kertoimet(0);
    keno_valinta_korttiin(VALINTA_XY, valinta_x == ARVOT_N - 1,
                          valinta_y == 3);
}

#define IMAGE_SIZE(w, h) ((w) * (h) * 4 / (PPB))

void keno_aloita_peli(int saldo) {
    alusta_sekoitin();
    jatka = 1;
    panos = 1;
    pelit = saldo;
    jokeri = 0;
    voitot = 0;
    voitto = 0;
    srand(time(NULL));

    if (tickskip < 2)
        raitatila = calloc(IMAGE_SIZE(RAITATILA_L, RAITATILA_K), 1);
    keno_alusta_tila(TK_ESIT);
    valot_efekti();
}

static void keno_lopeta(void) {
    if (raitatila) free(raitatila);
    raitatila = NULL;
    pelit += voitot + voitto;
    jatka = 0;
}

static int keraa_kortit(void) {
    short speed = SPEED_MUL(24);
    short offset = pakka_x - VALINTA_X;
    short x2 = pakka_x + KORTTI_L;
    if (anim < 15) return 0;
    if (offset > speed) offset = speed;
    if (!offset) return 1;
    if (pakka_x == PAKKA_X && kenotila != TK_SIIRTO3A)
        toista_aani(AANI_KORTTIEN_PUDOTUS);
    piirra_suorakulmio(pakka_x, PAKKA_Y, x2 - pakka_x + 8, KORTTI_K, 0);
    pakka_x -= offset;
    if (pakka_x == PAKKA_X)
        piirra_kuva(pakka_x, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_cache1);
    else if (frameskip < 2)
        piirra_kuva_suikale(pakka_x, PAKKA_Y, KORTTI_L, KORTTI_K,
                            0, 0, KORTTI_L, KORTTI_K, kortti_cache1, 1);
    else
        piirra_selka(pakka_x, PAKKA_Y);
    paivita_alue(pakka_x, PAKKA_Y, x2 - pakka_x, KORTTI_K);
    return 0;
}

static int shuffle_i = 0;
static signed char p0 = 0, p1 = 0, p2 = 0;
static signed char p0d = 0, p1d = 0, p2d = 0;
static int pakan_sekoitus(void) {
    char i, maxv = MIN(tickskip + 1, 4);
    signed char pmin, pmax;
    for (i = 0; i < SPEED_MUL(2); ++i)
        if (shuffle_i) sekoita_pakka(shuffle_i--);

    pmin = pmax = p0;
    if (p1 < pmin) pmin = p1;
    if (p1 > pmax) pmax = p1;
    if (p2 < pmin) pmin = p2;
    if (p2 > pmax) pmax = p2;

    p0 += p0d * maxv;
    if (p0 >= 8) p0d = -1, p0 = 16 - p0;
    if (p0 < 0) p0 = 0;
    if (p0 == 0) p0d = 0;
    if (p0 < pmin) pmin = p0;
    if (p0 > pmax) pmax = p0;

    p1 += p1d * maxv;
    if (p1 >= 8) p1d = -1, p1 = 16 - p1;
    if (p1 < 0) p1 = 0;
    if (p1 == 0) p1d = 0;
    if (p1 < pmin) pmin = p1;
    if (p1 > pmax) pmax = p1;

    p2 += p2d * maxv;
    if (p2 >= 8) p2d = -1, p2 = 16 - p2;
    if (p2 < 0) p2 = 0;
    if (p2 == 0) p2d = 0;
    if (p2 < pmin) pmin = p2;
    if (p2 > pmax) pmax = p2;

    piirra_suorakulmio(VALINTA_X, PAKKA_Y, KORTTI_L + 16 * 8, KORTTI_K, 0);
    piirra_selka_peitetty(VALINTA_X + 8 * p0, VALINTA_X + 8 * p1, PAKKA_Y);
    piirra_selka_peitetty(VALINTA_X + 8 * p1, VALINTA_X + 8 * p2, PAKKA_Y);
    piirra_selka(VALINTA_X + 8 * p2, PAKKA_Y);
    paivita_alue(VALINTA_X + 8 * pmin, PAKKA_Y,
                KORTTI_L + 8 * (pmax - pmin + 1), KORTTI_K);

    if (ANIM_EQ(1) || ANIM_EQ(16) || ANIM_EQ(31)) p0d = 1;
    if (ANIM_EQ(6) || ANIM_EQ(21) || ANIM_EQ(36)) p1d = 1;
    if (ANIM_EQ(11) || ANIM_EQ(26) || ANIM_EQ(41)) p2d = 1;
    
    if (anim >= 66 && p0 == 0 && p1 == 0 && p2 == 0) {
        while (shuffle_i) sekoita_pakka(shuffle_i--);
        return 1;
    }
    return 0;
}

static int jaa_kortteja(void) {
    short x = VALINTA_X + (unsigned short)jaettu * JAKO_L;
    if (jakoaika <= 0) {
        int i;
        kortti_t k;
        jakoaika = 15;
        if (pakka_x != x) {
            paivita_alue(pakka_x, PAKKA_Y, KORTTI_L + (x - pakka_x), KORTTI_K);
            piirra_suorakulmio(pakka_x, PAKKA_Y, x - pakka_x, KORTTI_K, 0);
        }

        if (jaettu++)
            piirra_kuva(x - JAKO_L, PAKKA_Y,
                        KORTTI_L, KORTTI_K, kortti_cache);
        pakka_x = x;
        k = jaa_kortti();
        if (k == K_JOKERI && osumat < valintoja)
            ++osumat;
        else
            for (i = 0; i < valintoja; ++i) {
                if (valitut_kortit[i] == k) {
                    osumakortit[i] = 1;
                    if (osumat < valintoja) ++osumat;
                    break;
                }
            }

        piirra_kortti(x, PAKKA_Y, k);
        sailyta_kuva(x, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_cache);
        piirra_selka(x, PAKKA_Y);
    } else if (pakka_x < x) {
        short x0 = x - JAKO_L;
        short dx;
        short uusi_pakka_x = pakka_x + SPEED_MUL(4);
        if (uusi_pakka_x > x) uusi_pakka_x = x;
        dx = uusi_pakka_x - pakka_x;
        pyyhi_kortti(pakka_x, PAKKA_Y);
        piirra_kuva(x0, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_cache);
        if (tickskip < 2)
            piirra_kuva_suikale(uusi_pakka_x, PAKKA_Y, KORTTI_L, KORTTI_K,
                            0, 0, KORTTI_L, KORTTI_K, kortti_cache1, 1);
        else
            piirra_selka(MIN(uusi_pakka_x + 4, x), PAKKA_Y);
        paivita_alue(pakka_x, PAKKA_Y, KORTTI_L + dx, KORTTI_K);
        pakka_x = uusi_pakka_x;
    }

    jakoaika -= SPEED_MUL(1);
    return jaettu >= JAKO_KORTTEJA;
}

static int jaa_tuplaus_kortti(void) {
    short old_pakka_x = pakka_x;
    int i;
    if (pakka_x >= PAKKA_X) return 1;

    for (i = 0; i <= tickskip; ++i) {
        short nopeus = 16;
        if (pakka_x > TUPLAUS_X - KORTTI_K / 2
                    && pakka_x < TUPLAUS_X + KORTTI_K / 2)
            nopeus = 4;
        pakka_x += nopeus;
        if (pakka_x >= PAKKA_X) {
            pakka_x = PAKKA_X;
            break;
        }
    }
    paivita_alue(old_pakka_x, PAKKA_Y,
                pakka_x - old_pakka_x + KORTTI_L, KORTTI_K);
    pyyhi_kortti(old_pakka_x, PAKKA_Y);
    if (pakka_x > TUPLAUS_X && old_pakka_x < TUPLAUS_X + KORTTI_L)
        piirra_selka(TUPLAUS_X, PAKKA_Y);
    if (tickskip < 2)
        piirra_kuva_suikale(pakka_x, PAKKA_Y, KORTTI_L, KORTTI_K,
                            0, 0, KORTTI_L, KORTTI_K, kortti_cache1,
                            TUPLAUS_X < pakka_x && pakka_x < TUPLAUS_X + KORTTI_L);
    else
        piirra_selka(pakka_x, PAKKA_Y);
    return 0;
}

void keno_alusta_tila(enum PelitilaKeno t) {
    switch (t) {
    case TK_VALINTA: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.valinta, 1);
        VALO_VILKKU(valot.panos, 1);
        if (voitot > 0)
            VALO_PAALLE(valot.voitot);
        if (!pelit && !voitot && !voitto) {
            keno_alusta_tila(TK_KONKKA);
            toista_musiikki_oletus(MUSA_KONKKA);
            return;
        } else if (panos > pelit + voitot) {
            panos = pelit + voitot;
            paivita_ylapalkki();
        } else if (pelit + voitot > 999999) {
            keno_alusta_tila(TK_HUOLTO);
            return;
        }
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        valintoja = 0;
        piirra_keno_kertoimet(0);
        inactive = 0;
    } break;
    case TK_PANOS: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.panos, 1);
        if (voitot > 0)
            VALO_PAALLE(valot.voitot);
        if (!pelit && !voitot && !voitto) {
            keno_alusta_tila(TK_KONKKA);
            return;
        } else if (panos > pelit + voitot) {
            panos = pelit + voitot;
            paivita_ylapalkki();
        } else if (pelit + voitot > 999999) {
            keno_alusta_tila(TK_HUOLTO);
            return;
        }
        VALO_VILKKU(valot.jako, 1);
        VALO_VILKKU(valot.peru, 1);
        VALO_VILKKU_ALUSTA();
        paivita_alapalkki();
        paivita_palkki();
        piirra_keno_kertoimet(0);
        inactive = 0;
    } break;
    case TK_SIIRTO0A: {
        int i, j;
        kaikki_valot_pois();
        VALO_VILKKU(valot.valinta, 1);
        VALO_VILKKU(valot.panos, 1);
        siirto_kortit = 0;
        valintoja = 0;
        valmistele_ruutu_peli();
        piirra_selka(PAKKA_X, PAKKA_Y);
        piirra_keno_kertoimet(0);
        for (i = 0; i < 4; ++i) {
            kortti_t k = i << 4;
            short y = VALINTA_Y + VALINTA_KH * i;
            for (j = 0; j < ARVOT_N; ++j) {
                short x = VALINTA_X + VALINTA_KW * j;
                piirra_kortti(x, y, k | j);
            }
        }
        paivita_alue(VALINTA_X, VALINTA_Y, RAITATILA_L, RAITATILA_K);
        keno_alusta_tila(TK_VALINTA);
        break;
    } break;
    case TK_SIIRTO0B: {
        int i, j;
        kaikki_valot_pois();
        VALO_VILKKU(valot.valinta, 1);
        VALO_VILKKU(valot.panos, 1);
        valmistele_ruutu_peli();
        for (i = 0; i < 4; ++i) {
            kortti_t k = i << 4;
            short y = VALINTA_KH * i;
            for (j = 0; j < ARVOT_N; ++j) {
                short x = VALINTA_KW * j;
                piirra_kortti(x, y, k | j);
            }
        }
        sailyta_kuva(0, 0, RAITATILA_L, RAITATILA_K, raitatila);
        osumat = siirto_kortit = sekoitusluku(RAITA_LKM);
        valmistele_ruutu_peli();
        piirra_selka(PAKKA_X, PAKKA_Y);
        piirra_keno_kertoimet(0);
    } break;
    case TK_SIIRTO1: {
        int i, j, k = 0;
        kaikki_valot_pois();
        VALO_PAALLE(valot.jako);
        siirto_kortit = RAITATILA_L + VALINTA_X - 8;
        kortti_caches[0] = kortti_cache;
        kortti_caches[1] = kortti_cache1;
        kortti_caches[2] = kortti_cache2;
        kortti_caches[3] = kortti_cache3;
        kortti_caches[4] = kortti_cache4;
        for (i = 1; i < valintoja; ++i) {
            j = i;
            while (j && valitut_kortit[j - 1] > valitut_kortit[j]) {
                kortti_t tmp = valitut_kortit[j - 1];
                valitut_kortit[j - 1] = valitut_kortit[j];
                valitut_kortit[j] = tmp;
                --j;
            }
        }
        if (raitatila) {
            sailyta_kuva(VALINTA_X, VALINTA_Y,
                         RAITATILA_L, RAITATILA_K, raitatila);
            piirra_suorakulmio(VALINTA_X, VALINTA_Y,
                               RAITATILA_L, RAITATILA_K, 0);
            for (j = 0; j < valintoja; ++j) {
                short x, y;
                x = VALINTA_X + KARVO(valitut_kortit[j]) * VALINTA_KW;
                y = VALINTA_Y + KMAA(valitut_kortit[j]) * VALINTA_KH;
                piirra_kortti(x, y, valitut_kortit[j]);
                sailyta_kuva(x, y, KORTTI_L, KORTTI_K, kortti_caches[j]);
            }
            piirra_kuva(VALINTA_X, VALINTA_Y,
                        RAITATILA_L, RAITATILA_K, raitatila);
        } else {
            for (i = 0; i < valintoja; ++i) {
                pyyhi_kortti(PAKKA_X, PAKKA_Y);
                piirra_kortti(PAKKA_X, PAKKA_Y, valitut_kortit[i]);
                sailyta_kuva(PAKKA_X, PAKKA_Y,
                             KORTTI_L, KORTTI_K, kortti_caches[i]);
            }
            pyyhi_kortti(PAKKA_X, PAKKA_Y);
            piirra_selka(PAKKA_X, PAKKA_Y);
        }
    } break;
    case TK_SIIRTO2: {
        int i;
        for (i = 0; i < valintoja; ++i) {
            short x, y;
            kortti_t k = valitut_kortit[i];
            kortit_x[i] = x = VALINTA_X + KARVO(k) * VALINTA_KW;
            kortit_y[i] = y = VALINTA_Y + KMAA(k) * VALINTA_KH;
            pyyhi_kortti(PAKKA_X, PAKKA_Y);
            piirra_kortti(PAKKA_X, PAKKA_Y, k);
            sailyta_kuva(PAKKA_X, PAKKA_Y,
                            KORTTI_L, KORTTI_K, kortti_caches[i]);
        }
        pyyhi_kortti(PAKKA_X, PAKKA_Y);
        piirra_selka(PAKKA_X, PAKKA_Y);
    } break;
    case TK_SIIRTO3A: {
        int i, j;
        kaikki_valot_pois();
        VALO_VILKKU(valot.valinta, 1);
        VALO_VILKKU(valot.panos, 1);
        siirto_kortit = 0;
        valmistele_ruutu_peli();
        piirra_selka(PAKKA_X, PAKKA_Y);
        piirra_keno_kertoimet(0);
        paivita_alue(VALINTA_X, VALINTA_Y, RAITATILA_L, RAITATILA_K);
        pakka_x = PAKKA_X;
        valinta_x = 6;
        valinta_y = 1;
        break;
    } break;
    case TK_SIIRTO3B:
        break;
    case TK_JAKO1: {
        int i;
        piirra_suorakulmio(alut_x[valintoja], VALINTA_Y,
                           valit_x[valintoja] * (valintoja - 1) + KORTTI_L,
                           KORTTI_K, 0);
        for (i = 0; i < valintoja; ++i) {
            short x = alut_x[valintoja] + valit_x[valintoja] * i;
            piirra_kortti(x, VALINTA_Y, valitut_kortit[i]);
        }
        paivita_alue(alut_x[valintoja], VALINTA_Y,
                     valit_x[valintoja] * (valintoja - 1) + KORTTI_L,
                     KORTTI_K);
        pakka_x = PAKKA_X;
        alusta_pakka();
        piirra_selka(PAKKA_X, PAKKA_Y);
        sailyta_kuva(PAKKA_X, PAKKA_Y,
                        KORTTI_L, KORTTI_K, kortti_cache1);
        PYSAYTA_AANET();
    } break;
    case TK_JAKO2: {
        jokeri = jokeri_saatavilla && panos >= jokeri_minimipanos;
        tuplaus = 0;
        shuffle_i = jokeri ? 52 : 51;
        p0 = p1 = p2 = p0d = p1d = p2d = 0;
        toista_aani(AANI_SEKOITA);
    } break;
    case TK_JAKO3: {
        tuplaus_ctr = 0;
        jaettu = 0;
        jakoaika = 0;
        osumat = 0;
        fast_memset(osumakortit, 0, sizeof(osumakortit));
    } break;
    case TK_JAKO4: {
        koputus_anim = 0;
        siirto_kortit = 0;
        toista_musiikki_oletus(MUSA_KOPUTUS);
    } break;
    case TK_JAKO5: {
    } break;
    case TK_PERU1: {
        kaikki_valot_pois();
        VALO_PAALLE(valot.peru);
        paivita_alapalkki();
        pakka_x = PAKKA_X;
        pyyhi_kortti(PAKKA_X, PAKKA_Y);
        piirra_selka(PAKKA_X, PAKKA_Y);
        sailyta_kuva(PAKKA_X, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_cache);
    } break;
    case TK_PERU2: {
        int j;
        for (j = 0; j < valintoja; ++j) {
            kortit_x[j] = alut_x[valintoja] + valit_x[valintoja] * j;
            pyyhi_kortti(PAKKA_X, PAKKA_Y);
            piirra_kortti(PAKKA_X, PAKKA_Y, valitut_kortit[j]);
            sailyta_kuva(PAKKA_X, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_caches[j]);
        }
        pyyhi_kortti(PAKKA_X, PAKKA_Y);
        siirto_kortit = 0;
    } break;
    case TK_TUPLA1: {
        kaikki_valot_pois();
        VALO_PAALLE(valot.tuplaus);
        voitto_disp = voitto;
        paivita_alapalkki();
        piirra_selka(PAKKA_X, PAKKA_Y);
        sailyta_kuva(PAKKA_X, PAKKA_Y,
                        KORTTI_L, KORTTI_K, kortti_cache1);
    } break;
    case TK_TUPLA2: {
        jokeri = jokeri_saatavilla && panos >= jokeri_minimipanos;
        tuplaus = 0;
        shuffle_i = jokeri ? 52 : 51;
        p0 = p1 = p2 = p0d = p1d = p2d = 0;
        toista_aani(AANI_SEKOITA);
    } break;
    case TK_TUPLA3: {
        tuplaus = 1;
        tuplaus_banner_x = 50;
        paivita_alapalkki();
        paivita_palkki();
    } break;
    case TK_TUPLA4: {
        voitto *= 2;
        update_voitto_hitaus(voitto, 0);
    } break;
    case TK_TUPLAUS: {
        /* toista_musiikki(MUSA_TUPLAUS, 120 + tuplaus_ctr * 24); */
        int vj = voitto, vk = 0;
        kaikki_valot_pois();
        VALO_VILKKU(valot.tuplaus_pieni, 1);
        VALO_VILKKU(valot.tuplaus_suuri, 2);
        VALO_VILKKU_ALUSTA();
        paivita_alapalkki();
        paivita_palkki();
        while (vj * 2 <= paavoitto && vk < 5)
            vj *= 2, ++vk;
        toista_musiikki(MUSA_TUPLAUS, 240 - vk * 24);
        musan_toisto = 1;
    } break;
    case TK_TUPLAOK: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.voitot, 1);
        if (voitto * 2 <= paavoitto)
            VALO_VILKKU(valot.tuplaus, 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        paivita_alapalkki();
        ++tuplaus_ctr;
        /* toista_musiikki(MUSA_VOITTO, 96 + tuplaus_ctr * 24); */
        toista_musiikki(MUSA_KOPUTUS, 144 + tuplaus_ctr * 20);
        musan_toisto = 0;
    } break;
    case TK_TUPLAEI: {
        toista_musiikki_oletus(AANI_TUPLAUS_HAVITTY);
        musan_toisto = 0;
        voitto = voitto_disp = 0;
        keno_piirra_alapalkki_vain_voitto(1);
        osumat = 0;
    } break;
    case TK_VOITTO: {
        kaikki_valot_pois();
        anim = 0;
        piirra_keno_kertoimet(0);
        paivita_palkki();
        musan_toisto = 0;
        voitto_vilkku = 0;
        if (kenotila != TK_VOITTO)
            alapalkki_fade = 1;
    } break;
    case TK_VSIIRTO: {
        oli_paavoitto = voitto * 2 >= paavoitto;
        voitto_disp = voitto;
        palswap = 0;
        update_voitto_hitaus(voitto, oli_paavoitto);
        if (kenotila == TK_VOITTO)
            piirra_keno_kertoimet(1);
    } break;
    case TK_KONKKA: {
        assert(!voitot && !voitto);
        toista_aani(AANI_PANOS1 + (panos % 5));
        kaikki_valot_pois();
        piirra_palkki_napit();
        paivita_alue(0, GAREA_HEIGHT,
                     PLANE_WIDTH, PLANE_HEIGHT - GAREA_HEIGHT);
        vaihda_keltaiseen_palettiin();
        if (!lisarahat_ok) {
            alapalkki_fade = 0;
            piirra_alapalkki();
            piirra_teksti_keski(320, ALAPALKKI_Y + 10, 1, 0,
                            english ? "GAME OVER" : "PEEAA", 0);
        }
    } break;
    case TK_ESIT: {
        char i, y, x;
        const unsigned char *p = logokartta;
        valmistele_ruutu_esittely();
        alapalkki_fade = 0;
        kaikki_valot_pois();
        piirra_palkki_napit();
        for (i = 0; i < 25; ++i) {
            piirra_selka((rand() % (GAREA_WIDTH - KORTTI_L - 64) + 32) & ~7,
                        (rand() % (GAREA_HEIGHT - KORTTI_K - 64) + 32) & ~7);
        }
        piirra_teksti(GAREA_WIDTH + 16, 16,
            15, 1, "VIDEOPOKERI-DOS/386", 0);
        piirra_teksti_oikea(GAREA_WIDTH - 16, GAREA_HEIGHT - 32,
            15, 1, "ORIGINAL 1986 RAY", 0);
        piirra_teksti_oikea(GAREA_WIDTH - 16, GAREA_HEIGHT - 16,
            15, 1, "DOS 2021-2025 RISTIJÄTKÄ", 0);
        for (y = 0; y < 12; ++y) {
            for (x = 0; x < 36;) {
                unsigned char c = *p++;
                for (i = 0; i < 8; ++i) {
                    if (c & 0x80)
                        piirra_suorakulmio(32 + x * 16, 80 + y * 16, 16, 16, 12);
                    c <<= 1;
                    ++x;
                }
            }
        }
        paivita_alue(0, 0, GAREA_WIDTH, GAREA_HEIGHT);
        inactive = 600;
    } break;
    case TK_HUOLTO: {
        kaikki_valot_pois();
        paivita_valot();
        alapalkki_fade = 0;
        piirra_alapalkki();
        piirra_teksti_keski(320, ALAPALKKI_Y + 10, 1, 0,
                          english ? "GET THE DEALER" : "HAE PELINHOITAJA", 0);
        vaihda_punaiseen_palettiin();
        toista_musiikki_oletus(MUSA_TUPLAUS);
        musan_toisto = 1;
        lisarahat_ok = 0;
    } break;
    }
    kenotila = t;
    anim = 0;
    new_mode = 1;
}

static void draw_konkka_line(short y) {
    if (y < 0 || y >= GAREA_HEIGHT) return;
    if (y < 160 || y >= 192) {
        fast_memset(taso0 + STRIDE * y, 0, STRIDE);
        fast_memset(taso1 + STRIDE * y, 0, STRIDE);
        fast_memset(taso2 + STRIDE * y, 0, STRIDE);
        fast_memset(taso3 + STRIDE * y, 0, STRIDE);
    } else {
        fast_memset(taso0 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso0 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
        fast_memset(taso1 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso1 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
        fast_memset(taso2 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso2 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
        fast_memset(taso3 + STRIDE * y, 0, 240 / PPB);
        fast_memset(taso3 + STRIDE * y + 400 / PPB, 0, 240 / PPB);
    }
    paivita_alue(0, y, GAREA_WIDTH, 1);
}

static long laske_kohti(long nyt, long tavoite) {
    long erotus = tavoite - nyt;
    if (erotus >= 500)
        return 100;
    else if (erotus >= 200)
        return 10;
    else if (erotus >= 50)
        return 5;
    else if (erotus > 0)
        return 1;
    else
        return 0;
}

static void piirra_keno_osuma(short x, short y) {
    int o, ox, oy, sstride, pstride;
    const unsigned char *s = G_kenoosuma;
    unsigned char m, *p0, *p1, *p2, *p3;

    x >>= 3;
    o = y * STRIDE + x;
    pstride = STRIDE - (OSUMA_L >> 3);

    p0 = taso0 + o, p1 = taso1 + o, p2 = taso2 + o, p3 = taso3 + o;

    for (oy = 0; oy < OSUMA_K; ++oy) {
        for (ox = 0; ox < (OSUMA_L >> 3); ++ox) {
            m = *s++;
            *p0++ &= ~m, *p1++ |= m, *p2++ &= ~m, *p3++ &= ~m;
        }
        p0 += pstride, p1 += pstride, p2 += pstride, p3 += pstride;
    }
}

static void keno_aja_peli_valinta(void) {
    char ox = valinta_x, oy = valinta_y;
    if (napit_kaikki.mika_tahansa)
        inactive = 0;
    else
        inactive += SPEED_MUL(1);
    if (napit.panos) {
        panos = (panos % maksimipanos) + 1;
        if (panos > pelit + voitot)
            panos = 1;
        toista_aani(AANI_PANOS1 + ((panos - 1) % 5));
        paivita_ylapalkki();
        piirra_keno_kertoimet(0);
    }
    if (voitot && napit.voitot) 
        keno_lopeta();
    if (napit.valinta)
        keno_valitse_kortti();
    if (oy > 0 && napit.ylos)
        --oy;
    if (oy < 3 && napit.alas)
        ++oy;
    if (ox > 0 && napit.vasen)
        --ox;
    if (ox < ARVOT_N - 1 && napit.oikea)
        ++ox;
    if (valinta_x != ox || valinta_y != oy) {
        piirra_kortti_vynurkka(VALINTA_XY, VALINTA_KORTTI, 0);
        paivita_alue(VALINTA_XY, VALINTA_KW, VALINTA_KH);
        valinta_x = ox, valinta_y = oy;
        toista_aani(AANI_KORTTI_KAANTYY);
        valinta_vilkku_anim = anim & 31;
        valinta_vilkku = 0;
    }
    if ((anim & 31) == valinta_vilkku_anim) {
        valinta_vilkku = !valinta_vilkku;
        piirra_kortti_vynurkka(VALINTA_XY, VALINTA_KORTTI,
                                valinta_vilkku ? 14 : 0);
        paivita_alue(VALINTA_XY, VALINTA_KW, VALINTA_KH);
    }
    if (inactive >= 3600) {
        kaikki_valot_pois();
        valot_efekti();
        piirra_palkki_napit();
        keno_alusta_tila(TK_ESIT);
    }
    if (valintoja && pelit + voitot >= panos && napit.jako) {
        piirra_kortti_vynurkka(VALINTA_XY, VALINTA_KORTTI, 0);
        kaikki_valot_pois();
        VALO_PAALLE(valot.jako);
        paivita_palkki();
        if (pelit <= panos) {
            voitot -= panos - pelit;
            pelit = 0;
        } else
            pelit -= panos;
        paivita_ylapalkki();
        paivita_alapalkki();
        keno_alusta_tila(TK_SIIRTO1);
    } else if (valintoja && napit.peru) {
        int i;
        for (i = 0; i < valintoja; ++i) {
            valinta_x = KARVO(valitut_kortit[i]);
            valinta_y = KMAA(valitut_kortit[i]);
            keno_valinta_pois_kortista(VALINTA_XY, valinta_x == ARVOT_N - 1,
                                    valinta_y == 3);
        }
        valintoja = 0;
        VALO_POIS(valot.jako);
        VALO_POIS(valot.peru);
        toista_aani(AANI_VALINTA);
        piirra_keno_kertoimet(0);
        piirra_palkki_napit();
    }
}

static void keno_aja_peli_jako4(void) {
    int i, a;
    short x = pakka_x;
    for (i = 0; i <= tickskip; ++i) {
        a = koputus_anim >> 1;
        ufrac += (a * a + (anim >= 160 ? (anim - 160) << 11 : 1)) >> 1;
        pakka_x += ufrac >> 12;
        ufrac &= 0xFFF;
        ++koputus_anim;
    }
    if (pakka_x > PAKKA_X)
        pakka_x = PAKKA_X;
    if (pakka_x != x) {
        short x0 = VALINTA_X + (JAKO_KORTTEJA - 1) * JAKO_L;
        short dx = pakka_x - x;
        short kx = x0 + KORTTI_L;
        short xb, x3;
        if (tickskip < 2) {
            xb = x, x3 = pakka_x;
            piirra_suorakulmio(xb, PAKKA_Y, dx, KORTTI_K, 0);
        } else {
            xb = x & ~7, x3 = pakka_x & ~7;
            piirra_suorakulmio(xb, PAKKA_Y, dx + 8, KORTTI_K, 0);
        }
        if (x <= x0 + KORTTI_L) 
            piirra_kuva(x0, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_cache);
        if (tickskip < 2) {
            piirra_kuva_suikale(x3, PAKKA_Y, KORTTI_L, KORTTI_K,
                                0, 0, KORTTI_L, KORTTI_K,
                                kortti_cache1, pakka_x < kx);
        } else {
            piirra_selka(x3, PAKKA_Y);
        }
        paivita_alue(xb, PAKKA_Y, KORTTI_L + dx + 8, KORTTI_K);
        if (pakka_x == PAKKA_X) {
            ufrac = 0;
            for (a = 0; a < valintoja; ++a) {
                if (osumakortit[a]) {
                    short lx = alut_x[valintoja] + a * valit_x[valintoja]
                                            + (KORTTI_L - OSUMA_L) / 2;
                    short ly = VALINTA_Y + (KORTTI_K - OSUMA_K) / 2;
                    piirra_keno_osuma(lx, ly);
                    paivita_alue(lx, ly, OSUMA_L, OSUMA_K);
                }
            }

            pyyhi_kortti(PAKKA_X, PAKKA_Y);
            piirra_selka(PAKKA_X, PAKKA_Y);
            paivita_alue(PAKKA_X, PAKKA_Y, KORTTI_L, KORTTI_K);
            if (osumat && keno_kertoimet[valintoja - osumat]) {
                voitto = panos * keno_kertoimet[valintoja - osumat];
                if (voitto > paavoitto) voitto = paavoitto;
                update_voitto_hitaus(voitto, 0);
                keno_alusta_tila(TK_VOITTO);
            } else {
                toista_aani(AANI_KENO_HAVITTY);
                keno_alusta_tila(TK_JAKO5);
            }
        }
    }
}

static void keno_aja_peli_peru2(void) {
    if (siirto_kortit < valintoja) {
        short x0 = kortit_x[siirto_kortit];
        short x1 = x0 - SPEED_MUL(24);
        pyyhi_kortti(x0, VALINTA_Y);
        kortit_x[siirto_kortit] = x1;
        if (siirto_kortit + 1 < valintoja
                    && x0 + KORTTI_L >= kortit_x[siirto_kortit + 1])
            piirra_kuva(kortit_x[siirto_kortit + 1], VALINTA_Y,
                        KORTTI_L, KORTTI_K, kortti_caches[siirto_kortit + 1]);
        if (x1 >= -KORTTI_L) {
            short x2 = MAX(0, x1);
            short x3 = MAX(0, x0 + KORTTI_L);
            piirra_kuva_rajaa(x2, VALINTA_Y, KORTTI_L + x1 - x2, KORTTI_K,
                        x2 - x1, 0, KORTTI_L, KORTTI_K,
                        kortti_caches[siirto_kortit]);
            paivita_alue(x2, VALINTA_Y, MAX(0, x3) - x2, KORTTI_K);
        } else {
            pyyhi_kortti(0, VALINTA_Y);
            paivita_alue(0, VALINTA_Y, KORTTI_L, KORTTI_K);
            ++siirto_kortit;
        }
    } else {
        valintoja = 0;
        keno_alusta_tila(TK_SIIRTO0A);
    }
}

static void keno_aja_peli_internal(void) {
    switch (kenotila) {
    case TK_VALINTA: {
        keno_aja_peli_valinta();
        break;
    }
    case TK_PANOS: {
        if (napit.panos) {
            panos = (panos % maksimipanos) + 1;
            if (panos > pelit + voitot)
                panos = 1;
            toista_aani(AANI_PANOS1 + ((panos - 1) % 5));
            paivita_ylapalkki();
            piirra_keno_kertoimet(0);
            VALO_VILKKU(valot.jako, 1);
        }
        if (pelit + voitot >= panos && napit.jako) {
            kaikki_valot_pois();
            VALO_PAALLE(valot.jako);
            paivita_palkki();
            if (pelit <= panos) {
                voitot -= panos - pelit;
                pelit = 0;
            } else
                pelit -= panos;
            paivita_ylapalkki();
            paivita_alapalkki();
            keno_alusta_tila(TK_JAKO1);
        }
        if (voitot && napit.voitot)
            keno_lopeta();
        else if (napit.peru) {
            kaikki_valot_pois();
            VALO_PAALLE(valot.peru);
            keno_alusta_tila(TK_PERU1);
        }
        break;
    }
    case TK_SIIRTO0A: {
        keno_alusta_tila(TK_VALINTA);
        break;
    }
    case TK_SIIRTO0B: {
        int i;
        if (tickskip >= 2) {
            keno_alusta_tila(TK_SIIRTO0A);
            break;
        }
        for (i = 0; i < SPEED_MUL(32); ++i) {
            short x = (siirto_kortit >> 2) * RAITA_L;
            short y = (siirto_kortit & 3) * RAITA_K;
            piirra_kuva_suikale(VALINTA_X + x, VALINTA_Y + y, 1, RAITA_K,
                                x, y, RAITATILA_L, RAITATILA_K, raitatila, 0);
            siirto_kortit = (siirto_kortit + 1259) % RAITA_LKM;
            if (siirto_kortit == osumat) {
                keno_alusta_tila(TK_VALINTA);
                osumat = 0;
                break;
            }
        }
        paivita_alue(VALINTA_X, VALINTA_Y, RAITATILA_L, RAITATILA_K);
        break;
    }
    case TK_SIIRTO3A:
        if (keraa_kortit())
            keno_alusta_tila(TK_SIIRTO3B);
        break;
    case TK_SIIRTO3B: {
        short speed = SPEED_MUL(24);
        short offset = PAKKA_X - pakka_x;
        short x0 = pakka_x;
        if (offset > speed) offset = speed;
        if (!offset) {
            keno_alusta_tila(raitatila ? TK_SIIRTO0B : TK_SIIRTO0A);
            break;
        }
        piirra_suorakulmio(x0, PAKKA_Y, offset + 8, KORTTI_K, 0);
        pakka_x += offset;
        if (pakka_x == PAKKA_X)
            piirra_kuva(pakka_x, PAKKA_Y, KORTTI_L, KORTTI_K, kortti_cache1);
        else if (frameskip < 2)
            piirra_kuva_suikale(pakka_x, PAKKA_Y, KORTTI_L, KORTTI_K,
                                0, 0, KORTTI_L, KORTTI_K, kortti_cache1, 1);
        else
            piirra_selka(pakka_x, PAKKA_Y);
        paivita_alue(x0, PAKKA_Y, pakka_x - x0 + KORTTI_L, KORTTI_K);
        break;
    }
    case TK_SIIRTO1: {
        int i, j;
        for (i = 0; i < SPEED_MUL(2); ++i) {
            piirra_suorakulmio(siirto_kortit, VALINTA_Y, 8, RAITATILA_K, 0);
            for (j = 0; j < valintoja; ++j) {
                short x = VALINTA_X + KARVO(valitut_kortit[j]) * VALINTA_KW;
                if (x <= siirto_kortit && siirto_kortit < x + KORTTI_L) {
                    short y = VALINTA_Y + KMAA(valitut_kortit[j]) * VALINTA_KH;
                    piirra_kuva_rajaa(siirto_kortit, y, 8, KORTTI_K,
                                      siirto_kortit - x, 0, KORTTI_L,
                                      KORTTI_K, kortti_caches[j]);
                }
            }
            paivita_alue(siirto_kortit, VALINTA_Y, 8, RAITATILA_K);
            if (siirto_kortit == 0) {
                keno_alusta_tila(TK_SIIRTO2);
                break;
            }
            siirto_kortit -= 8;
        }
        break;
    }
    case TK_SIIRTO2: {
        int i, valmis = 1;
        short speed = SPEED_MUL(32);
        if (anim < 45) break;
        for (i = 0; i < valintoja; ++i) {
            short kohde_x = alut_x[valintoja] + valit_x[valintoja] * i;
            short erotus_x = kohde_x - (short)kortit_x[i];
            short erotus_y = VALINTA_Y - (short)kortit_y[i];
            if (erotus_x || erotus_y) {
                valmis = 0;
                if (erotus_x < -speed) erotus_x = -speed;
                if (erotus_x > +speed) erotus_x = +speed;
                if (erotus_y < -speed) erotus_y = -speed;
                if (erotus_y > +speed) erotus_y = +speed;
                pyyhi_kortti(kortit_x[i], kortit_y[i]);
                paivita_alue(kortit_x[i], kortit_y[i], KORTTI_L, KORTTI_K);
                kortit_x[i] += erotus_x, kortit_y[i] += erotus_y;
            }
        }
        for (i = 0; i < valintoja; ++i) {
            piirra_kuva(kortit_x[i], kortit_y[i], KORTTI_L, KORTTI_K,
                        kortti_caches[i]);
            paivita_alue(kortit_x[i], kortit_y[i], KORTTI_L, KORTTI_K);
        }
        if (valmis)
            keno_alusta_tila(TK_JAKO1);
        break;
    }
    case TK_JAKO1:
        if (keraa_kortit())
            keno_alusta_tila(TK_JAKO2);
        break;
    case TK_JAKO2: /* sekoita */
        if (pakan_sekoitus())
            keno_alusta_tila(TK_JAKO3);
        break;
    case TK_JAKO3:
        if (jaa_kortteja())
            keno_alusta_tila(TK_JAKO4);
        break;
    case TK_JAKO4: { /* tinnitys jäävistyy */
        keno_aja_peli_jako4();
        break;
    }
    case TK_JAKO5: {
        if (anim >= 30) keno_alusta_tila(TK_PANOS);
        break;
    }
    case TK_VOITTO: {
        if (!voitto_vilkku) {
            VALO_VILKKU(valot.voitot, 1);
            if (voitto * 2 <= paavoitto)
                VALO_VILKKU(valot.tuplaus, 1);
            VALO_VILKKU_ALUSTA();
            voitto_vilkku = 1;
            anim = 0;
            paivita_palkki();
        } else if ((anim & 31) <= tickskip) {
            vilkuta_voittokerrointa();
        }
        if (napit.voitot || voitto * 2 > paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            keno_alusta_tila(TK_VSIIRTO);
        }
        if (napit.tuplaus && voitto * 2 <= paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            keno_alusta_tila(TK_TUPLA1);
        } else if (voitto_disp < voitto && (anim % voitto_hitaus) <= tickskip) {
            voitto_disp += laske_kohti(voitto_disp, voitto);
            toista_aani(AANI_VOITONMAKSU);
            paivita_alapalkki_voitto();
        } else if (voitto_disp == voitto && !musan_toisto) {
            toista_musiikki(MUSA_VOITTO, 100 + tuplaus_ctr * 20);
            musan_toisto = 1;
        }
        break;
    }
    case TK_TUPLA1: /* kerää kortit */
        if (keraa_kortit())
            keno_alusta_tila(TK_TUPLA2);
        break;
    case TK_TUPLA2: /* sekoita */
        if (pakan_sekoitus())
            keno_alusta_tila(TK_TUPLA3);
        break;
    case TK_TUPLA3: /* jaa kortti */
        if (jaa_tuplaus_kortti())
            keno_alusta_tila(TK_TUPLA4);
        break;
    case TK_TUPLA4: /* panos tuplataan */
        if (voitto_disp < voitto && (anim % voitto_hitaus) <= tickskip) {
            voitto_disp += laske_kohti(voitto_disp, voitto);
            toista_aani(AANI_VOITONMAKSU);
            paivita_alapalkki_voitto();
        } else if (voitto_disp == voitto)
            keno_alusta_tila(TK_TUPLAUS);
        break;
    case TK_TUPLAUS: { /* arvaa */
        if (napit.tuplaus_pieni || napit.tuplaus_suuri) {
            int oikein = 0;
            kortti_t tuplaus_kortti = jaa_kortti();
            if (napit.tuplaus_pieni)
                oikein = KARVO(tuplaus_kortti) < 6;
            if (napit.tuplaus_suuri)
                oikein = KARVO(tuplaus_kortti) > 6;
            oikein |= tuplaus_kortti == K_JOKERI;
            piirra_kortti(TUPLAUS_X, PAKKA_Y, tuplaus_kortti);
            paivita_alue(TUPLAUS_X, PAKKA_Y, KORTTI_L, KORTTI_K);
            keno_alusta_tila(oikein ? TK_TUPLAOK : TK_TUPLAEI);
        }
        paivita_tuplaus_alapalkki();
        break;
    }
    case TK_TUPLAEI:
        if (anim >= 30) {
            if (alapalkki_fade == 8)
                alapalkki_fade = 9;
            else if (!alapalkki_fade) {
                keno_alusta_tila((pelit | voitot) ? TK_PANOS : TK_KONKKA);
            }
        } else
            paivita_tuplaus_alapalkki();
        break;
    case TK_TUPLAOK: {
        unsigned dur = 180 - tuplaus_ctr * 6;
        if (napit.tuplaus && voitto * 2 <= paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            keno_alusta_tila(TK_TUPLA1);
            break;
        }
        if (napit.voitot || voitto * 2 > paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            keno_alusta_tila(TK_VSIIRTO);
            break;
        }
        if (ANIM_EQ(dur)) {
            toista_musiikki(MUSA_VOITTO, 100 + tuplaus_ctr * 20);
            musan_toisto = 1;
        }
        break;
    }
    case TK_PERU1: {
        short speed = SPEED_MUL(16);
        short x2 = pakka_x + KORTTI_L;
        short x0, sw;
        if (pakka_x <= -KORTTI_L) {
            keno_alusta_tila(TK_PERU2);
            break;
        }
        pakka_x -= speed;
        x0 = pakka_x < 0 ? 0 : pakka_x;
        sw = MAX(0, x2 - pakka_x);
        piirra_suorakulmio(x0, PAKKA_Y, sw, KORTTI_K, 0);
        if (pakka_x >= -KORTTI_L) {
            piirra_kuva_rajaa(x0, PAKKA_Y, KORTTI_L + pakka_x - x0, KORTTI_K,
                        x0 - pakka_x, 0, KORTTI_L, KORTTI_K, kortti_cache);
        }
        paivita_alue(x0, PAKKA_Y, sw, KORTTI_K);
        break;
    }
    case TK_PERU2: {
        keno_aja_peli_peru2();
        break;
    }
    case TK_VSIIRTO: {
        if (oli_paavoitto && anim % 8 <= tickskip) {
            switch (palswap) {
            case 0: vaihda_liilaan_palettiin(); break;
            case 1: vaihda_vihreaan_palettiin(); break;
            }
            palswap = (palswap + 1) & 1;
        }
        if (anim % voitto_hitaus > tickskip) return;
        if (!voitto) {
            if (alapalkki_fade)
                alapalkki_fade = 9;
            if (oli_paavoitto) {
                palswap = 0;
                vaihda_liilaan_palettiin();
                toista_musiikki_oletus(MUSA_PAAVOITTO);
            }
            paivita_ylapalkki();
            paivita_alapalkki();
            voitto_kasi = -1;
            osumat = 0;
            keno_alusta_tila(oli_paavoitto ? TK_VLOPPU : TK_PANOS);
            oli_paavoitto = 0;
        } else {
            short kasvatus = laske_kohti(voitot, voitot + voitto);
            voitot += kasvatus;
            voitto -= kasvatus;
            voitto_disp = voitto;
            toista_aani(AANI_VOITONMAKSU);
            paivita_ylapalkki_voitot();
            paivita_alapalkki_voitto();
        }
        break;
    }
    case TK_VLOPPU: {
        if (anim >= 10) {
            piirra_keno_kertoimet(0);
            vaihda_normaaliin_palettiin();
            keno_alusta_tila(TK_PANOS);
        }
        break;
    }
    case TK_KONKKA:
        if (anim >= 300 && !lisarahat_ok)
            keno_lopeta();
        else if (pelit > 0) {
            vaihda_normaaliin_palettiin();
            keno_alusta_tila(TK_SIIRTO3A);
        }
        break;
    case TK_ESIT: {
        if (napit_kaikki.mika_tahansa) {
            lopeta_esittely();
            return;
        }
        if (inactive <= 0) {
            keno_alusta_tila(TK_ESIT);
        } else
            inactive -= SPEED_MUL(1);
        break;
    }
    case TK_HUOLTO:
        break;
    }
}

void keno_aja_peli(void) {
    ++tick;
    if (tickskip && tickskip_i++ < tickskip)
        return;
    tickskip_i = 0;
    if (alapalkki_fade & 7) {
        if (!(tick & 1)) {
            short i;
            for (i = 0; i <= tickskip && (alapalkki_fade & 7); ++i)
                alapalkki_fade = (alapalkki_fade + 1) & 15;
            paivita_alapalkki();
        }
        return;
    }

    if (skip_input_read)
        skip_input_read = 0;
    else
        lue_napit();
    if (napit_kaikki.lopeta) {
        keno_lopeta();
        return;
    }
    if (napit_kaikki.lisaa_panos_1)
        lisaa_panos(1);
    if (napit_kaikki.lisaa_panos_5)
        lisaa_panos(5);
    keno_aja_peli_internal();
    if (!new_mode)
        anim += SPEED_MUL(1);
    new_mode = 0;
}
