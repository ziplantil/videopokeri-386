
#include <dos.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "POKERI.H"
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

enum Pelitila tila;
char jatka;
int panos;
long pelit;
long voitot;
int voitto;
char jokeri;
static char voitto_vilkku = 0;
static char musan_toisto = 0;
static char skip_input_read = 0;
static int voitto_disp = 0;
static int voitto_speed = 0;
static int voitto_speed_mul = 0;
static char new_mode = 0;
static char card0zoom = 0;
char tickskip = 0;
static char tickskip_i = 0;
char frameskip = 0;
unsigned long anim = 0;
unsigned long tick = 0;
static char tuplaus = 0;
char alapalkki_fade = 0;
static kortti_t tuplaus_kortti = 0;
static char sailotty_kortti = 0;
static char kortti_sailotty = 0;
static short sailotty_miny = 0;
static short sailotty_maxy = CARDAREA_HEIGHT;
static char tuplaus_ctr = 0;
static char oli_paavoitto = 0;
static char palswap = 0;
static int card_fall_speed = 0;
static int inactive = 0;
static int present_y = 0;
static int present_hand = 0;
static int present_panos;

#define PAKKA_X 16
#define PAKKA_Y 48
#define KERAYS_Y 192
#define POYTA_X0 48
#define POYTA_XL 112
#define POYTA_Y 224
#define TUPLAUS_Y 208
#define POYTA_X(i) ((POYTA_X0) + ((POYTA_XL) * ((i) - 1)))

#define ANIM_EQ(n) ((anim >= (n)) && (anim <= ((n) + tickskip)))
#define ANIM_LE(n) (anim <= ((n) + tickskip))
#define SPEED_MUL(x) ((x) * (tickskip + 1))
#define PYSAYTA_AANET() (musan_toisto = 0, pysayta_aanet())

struct Pelikortti pelikortit[6] = { 
    { PAKKA_X, PAKKA_Y, 0, 1, 1, 0 }, /* selk„puoli */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 1, tyhj„ */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 2, tyhj„ */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 3, tyhj„ */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }, /* kortti 4, tyhj„ */
    { PAKKA_X, PAKKA_Y, 0, 0, 0, 0 }  /* kortti 5, tyhj„ */
};
struct Pelikortti pelikortit_v[6];
unsigned char *kortti_caches[6] = { 0 };

const char* logokartta = \
    "   X    X  X  XXXX   XXXXX   XXX    "
    "   X    X  X  X   X  X      X   X   "
    "    X  X   X  X   X  XXXXX  X   X   "
    "     X X   X  X   X  X      X   X   "
    "      X    X  XXXX   XXXXX   XXX    "
    "                                    "
    "                                    "
    "XXXX    XXX   X   X  XXXXX  XXXX   X"
    "X   X  X   X  X  X   X      X   X  X"
    "XXXX   X   X  XXX    XXXXX  XXXX   X"
    "X      X   X  X  X   X      X  X   X"
    "X       XXX   X   X  XXXXX  X   X  X";

#define PAIVITA_KORTTI(i) (pelikortit[i].muutettu = 1)

void paivita_kaikki_kortit(void) {
    PAIVITA_KORTTI(0);
    PAIVITA_KORTTI(1);
    PAIVITA_KORTTI(2);
    PAIVITA_KORTTI(3);
    PAIVITA_KORTTI(4);
    PAIVITA_KORTTI(5);
}

int lerp(int y1, int y2, int x1, int x, int x2) {
    if (x > x2) x = x2;
    return (y1 * (x2 - x) + y2 * (x - x1)) / (x2 - x1);
}

static const char *tuplaus_onnittelut[] = {
    "USKOMATONTA",
    "MESTARITUPLAUS",
    "ONNEKAS ARVAUS",
    "KYLMŽT HERMOT",
    "LUOVUTAN"
};
static const char *tuplaus_onnittelut_e[] = {
    "EXCELLENT",
    "NICELY DOUBLED",
    "WELL GUESSED",
    "NERVES OF STEEL",
    "I GIVE UP"
};
#define ONNITTELU_N (sizeof(tuplaus_onnittelut) / sizeof(tuplaus_onnittelut[0]))

const char *pelitila_nimi(enum Pelitila t) {
    switch (t) {
    case T_PANOS: return "T_PANOS";
    case T_VALITSE: return "T_VALITSE";
    case T_VOITTO: return "T_VOITTO";
    case T_TUPLAUS: return "T_TUPLAUS";
    case T_TUPLAOK: return "T_TUPLAOK";
    case T_TUPLAEI: return "T_TUPLAEI";
    case T_JAKO1: return "T_JAKO1";
    case T_JAKO2: return "T_JAKO2";
    case T_JAKO3: return "T_JAKO3";
    case T_UJAKO1: return "T_UJAKO1";
    case T_UJAKO2: return "T_UJAKO2";
    case T_UJAKO3: return "T_UJAKO3";
    case T_UJAKO4: return "T_UJAKO4";
    case T_TUPLA1: return "T_TUPLA1";
    case T_TUPLA2: return "T_TUPLA2";
    case T_TUPLA3: return "T_TUPLA3";
    case T_TUPLA4: return "T_TUPLA4";
    case T_VSIIRTO: return "T_VSIIRTO";
    case T_ESIT1: return "T_ESIT1";
    case T_ESIT2: return "T_ESIT2";
    default: abort(); return NULL;
    }
}

void paivita_alue_kortti(short x1, short y1, short x2, short y2) {
    if (y2 > CARDAREA_HEIGHT) y2 = CARDAREA_HEIGHT;
    if (y1 > CARDAREA_HEIGHT) y1 = CARDAREA_HEIGHT;
    if (y1 < y2)
        paivita_alue(x1, y1, x2 - x1, y2 - y1);
}

void paivita_kortit(void) {
    int i, j;
    struct Pelikortti *pk, *pv;
    char r = 0;
    static short rx1, ry1, rx2, ry2;
    static short tx1, ty1, tx2, ty2;

#define AUTO_RECT() do {                                        \
                if (pk->x < pv->x)                              \
                    tx1 = pk->x, tx2 = pv->x + KORTTI_L;        \
                else                                            \
                    tx1 = pv->x, tx2 = pk->x + KORTTI_L;        \
                if (pk->y < pv->y)                              \
                    ty1 = pk->y, ty2 = pv->y + KORTTI_K;        \
                else                                            \
                    ty1 = pv->y, ty2 = pk->y + KORTTI_K;        \
    } while (0);
#define COMMIT_RECT() do {                                      \
        if (!r)                                                 \
            ++r, rx1 = tx1, ry1 = ty1, rx2 = tx2, ry2 = ty2;    \
        else if (ry1 > ty2 || ty1 > ry2)                        \
            paivita_alue_kortti(tx1, ty1, tx2, ty2);            \
        else                                                    \
            rx1 = MIN(rx1, tx1), ry1 = MIN(ry1, ty1),           \
            rx2 = MAX(rx2, tx2), ry2 = MAX(ry2, ty2);           \
    } while (0);

    for (i = card0zoom ? 6 : 5; i >= 0; --i) {
        j = i == 6 ? 0 : i;
        pk = &pelikortit[j];
        pv = &pelikortit_v[j];
        if (pk->muutettu) {
            if (i == j)
                pk->muutettu = 0;
            if (j == 0 && card0zoom) {
                if (i) {
                    siirra_tyhja(pv->x, pv->y, pk->x, pk->y);
                    continue;
                } else {
                    piirra_selka(pk->x, pk->y);
                    AUTO_RECT();
                    COMMIT_RECT();
                }
            } else if (sailotty_kortti && sailotty_kortti == i
                    && kortti_sailotty && pv->x == pk->x && pv->y == pk->y) {
                if (sailotty_miny) {
                    short yo = sailotty_miny - pk->y;
                    piirra_kuva_rajaa_y(pk->x, sailotty_miny, KORTTI_L,
                                KORTTI_K - yo, KORTTI_K,
                                kortti_cache + yo * KORTTI_L / PPB,
                                sailotty_maxy);
                } else {
                    piirra_kuva_rajaa_y(pk->x, pk->y, KORTTI_L, KORTTI_K,
                                KORTTI_K, kortti_cache, sailotty_maxy);
                }
            } else if (pk->nakyva && pv->nakyva
                    && (pk->x != pv->x || pk->y != pv->y)) {
                if (pk->selka)
                    siirra_selka(pv->x, pv->y, pk->x, pk->y);
                else
                    siirra_kortti(pv->x, pv->y, pk->x, pk->y, pk->kortti);
                AUTO_RECT();
                COMMIT_RECT();
            } else if (pk->nakyva) {
                if (pk->selka)
                    piirra_selka(pk->x, pk->y);
                else
                    piirra_kortti(pk->x, pk->y, pk->kortti);
                tx1 = pk->x, ty1 = pk->y;
                tx2 = tx1 + KORTTI_L, ty2 = ty1 + KORTTI_K;
                COMMIT_RECT();
            } else if (pv->nakyva) {
                pyyhi_kortti(pv->x, pv->y);
                tx1 = pv->x, ty1 = pv->y;
                tx2 = tx1 + KORTTI_L, ty2 = ty1 + KORTTI_K;
                COMMIT_RECT();
            }
            if (sailotty_kortti && sailotty_kortti == i && (!kortti_sailotty
                        || pv->x != pk->x || pv->y != pk->y)) {
                sailyta_kuva(pk->x, pk->y, KORTTI_L, KORTTI_K, kortti_cache);
                kortti_sailotty = 1;
            }
            fast_memcpy(pv, pk, sizeof(struct Pelikortti));
        }
    }

    if (r)
        paivita_alue_kortti(rx1, ry1, rx2, ry2);
}

void aanitesti(void) {
    int n, kappale;
    for (;;) {
        if (english)
            printf("Secret sound test. Enter 1 - %d or 0 to quit\n",
                    AANI_AANTEN_LOPPU);
        else
            printf("Ž„nitesti. Sy”t„ numero 1 - %d, tai 0 lopettaa\n",
                    AANI_AANTEN_LOPPU);
        n = scanf("%u", &kappale);
        if (n <= 0)
            return;
        if (kappale <= 0)
            return;
        if (kappale < MUSA_MUSIEN_LOPPU)
            toista_musiikki_oletus(kappale);
        else
            toista_aani(kappale);
        puts(english ? "Playing... (press ESC to stop)"
                     : "Toistetaan (ESC lopettaa)...");
        anim = 0;
        while (!napit.lopeta && toistaa_aanta()) {
            lue_napit();
            paivita_soitto();
            tahti();
        }
        PYSAYTA_AANET();
        lue_napit();
    }
}

void aloita_peli(int saldo) {
    alusta_sekoitin();
    jatka = 1;
    panos = 1;
    pelit = saldo;
    jokeri = 0;
    voitot = 0;
    voitto = 0;
    srand(time(NULL));
    present_hand = rand() % KASI_LKM;
    alusta_tila(T_ESIT0);
    valot_efekti();
}

void keraa_kortit(enum Pelitila seuraava) {
    if (tuplaus) {
        if (ANIM_LE(16)) {
            pelikortit[3].x = lerp(POYTA_X(3), PAKKA_X, 0, anim, 16);
            pelikortit[3].y = lerp(TUPLAUS_Y, PAKKA_Y, 0, anim, 16);
            pelikortit[3].selka = 1;
            pelikortit[3].nakyva = anim < 16;
            PAIVITA_KORTTI(3);
        }
        if (ANIM_EQ(16)) {
            pelikortit[0].x = PAKKA_X;
            pelikortit[0].y = PAKKA_Y;
            alusta_tila(seuraava);
        }
        PAIVITA_KORTTI(0);
        return;
    }

    if (ANIM_LE(15)) {
        int x = pelikortit[0].x;
        if (x >= POYTA_X(1) && x < POYTA_X(1) + KORTTI_L)
            PAIVITA_KORTTI(1);
        if (x >= POYTA_X(2) && x < POYTA_X(2) + KORTTI_L)
            PAIVITA_KORTTI(2);
        if (x >= POYTA_X(3) && x < POYTA_X(3) + KORTTI_L)
            PAIVITA_KORTTI(3);
        pelikortit[0].x = lerp(PAKKA_X, POYTA_X(3), 0, anim, 15);
        pelikortit[0].y = lerp(PAKKA_Y, KERAYS_Y, 0, anim, 15);
        PAIVITA_KORTTI(0);
    }
    if (ANIM_EQ(16)) {
        pelikortit[0].x = POYTA_X(3);
        pelikortit[0].y = KERAYS_Y;
        PAIVITA_KORTTI(0);
    }
    if (10 <= anim && ANIM_LE(20)) {
        pelikortit[3].y = lerp(POYTA_Y, KERAYS_Y, 10, anim, 20);
        PAIVITA_KORTTI(3);
    }
    if (20 <= anim && ANIM_LE(30)) {
        if (!pelikortit[2].nakyva) anim += 10;
        pelikortit[2].x = lerp(POYTA_X(2), POYTA_X(3), 20, anim, 30);
        pelikortit[2].y = lerp(POYTA_Y, KERAYS_Y, 20, anim, 30);
        PAIVITA_KORTTI(2);
    }
    if (30 <= anim && ANIM_LE(40)) {
        if (!pelikortit[4].nakyva) anim += 10;
        pelikortit[4].x = lerp(POYTA_X(4), POYTA_X(3), 30, anim, 40);
        pelikortit[4].y = lerp(POYTA_Y, KERAYS_Y, 30, anim, 40);
        PAIVITA_KORTTI(4);
    }
    if (40 <= anim && ANIM_LE(50)) {
        if (!pelikortit[1].nakyva) anim += 10;
        pelikortit[1].x = lerp(POYTA_X(1), POYTA_X(3), 40, anim, 50);
        pelikortit[1].y = lerp(POYTA_Y, KERAYS_Y, 40, anim, 50);
        PAIVITA_KORTTI(1);
    }
    if (50 <= anim && ANIM_LE(60)) {
        if (!pelikortit[5].nakyva) anim += 10;
        pelikortit[5].x = lerp(POYTA_X(5), POYTA_X(3), 50, anim, 60);
        pelikortit[5].y = lerp(POYTA_Y, KERAYS_Y, 50, anim, 60);
        PAIVITA_KORTTI(5);
    }
    if (ANIM_EQ(60)) {
        int i;
        for (i = 1; i <= 5; ++i)
            pelikortit[i].nakyva = 0;
        paivita_kaikki_kortit();
    }
    if (anim <= 60 && anim % 10 <= tickskip)
        toista_aani(AANI_KORTTI_LIUKUU);
    if (60 <= anim && ANIM_LE(75)) {
        pelikortit[0].x = lerp(POYTA_X(3), PAKKA_X, 60, anim, 75);
        pelikortit[0].y = lerp(KERAYS_Y, PAKKA_Y, 60, anim, 75);
        PAIVITA_KORTTI(0);
    }
    if (ANIM_EQ(75)) {
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        alusta_tila(seuraava);
        PAIVITA_KORTTI(0);
    }
}

int shuffle_i = 0;
signed char p0 = 0, p1 = 0, p2 = 0;
signed char p0d = 0, p1d = 0, p2d = 0;
void pakan_sekoitus(int seuraava) {
    char i, maxv = MIN(tickskip + 1, 4);
    signed char pmin, pmax;
    for (i = 0; i < 2 * (tickskip + 1); ++i)
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

    piirra_suorakulmio(PAKKA_X, PAKKA_Y, KORTTI_L + 16 * 8, KORTTI_K, 0);
    piirra_selka_peitetty(PAKKA_X + 8 * p0, PAKKA_X + 8 * p1, PAKKA_Y);
    piirra_selka_peitetty(PAKKA_X + 8 * p1, PAKKA_X + 8 * p2, PAKKA_Y);
    piirra_selka(PAKKA_X + 8 * p2, PAKKA_Y);
    paivita_alue(PAKKA_X + 8 * pmin, PAKKA_Y,
                KORTTI_L + 8 * (pmax - pmin + 1), KORTTI_K);

    if (ANIM_EQ(1) || ANIM_EQ(16) || ANIM_EQ(31)) p0d = 1;
    if (ANIM_EQ(6) || ANIM_EQ(21) || ANIM_EQ(36)) p1d = 1;
    if (ANIM_EQ(11) || ANIM_EQ(26) || ANIM_EQ(41)) p2d = 1;
    
    if (anim >= 66 && p0 == 0 && p1 == 0 && p2 == 0) {
        assert(shuffle_i == 0),
        alusta_tila(seuraava), paivita_kaikki_kortit();
    }
}

kortti_t jaa_yksi_kortti(int i, int selka) {
    kortti_t kortti = jaa_kortti();
    RANGE_CHECK(i, 1, 6);
    pelikortit[i].x = pelikortit[0].x;
    pelikortit[i].y = pelikortit[0].y;
    pelikortit[i].nakyva = 1;
    pelikortit[i].selka = selka;
    pelikortit[i].kortti = kortti;
    poyta[i - 1] = kortti;
    PAIVITA_KORTTI(i);
    return kortti;
}

short jaa_i = 0;
short jaa_ctr = 0;
void jaa_kortit(void) {
    short i, x, landed = 1;
    for (i = 1; i <= 5; ++i) {
        if (pelikortit[i].nakyva && pelikortit[i].y < POYTA_Y) {
            pelikortit[i].y = MIN(pelikortit[i].y + SPEED_MUL(16), POYTA_Y);
            PAIVITA_KORTTI(i);
            landed = 0;
        }
    }

    if (jaa_i >= 6) {
        if (pelikortit[0].x == PAKKA_X) {
            if (landed) {
                alusta_tila(T_VALITSE);
                card0zoom = 0;
            }
            return;
        }
        pelikortit[0].x = x = MAX(pelikortit[0].x - SPEED_MUL(40), PAKKA_X);
        PAIVITA_KORTTI(0);
        for (i = 1; i <= 5; ++i) {
            if (pelikortit[i].selka && x < POYTA_X(6 - i)) {
                pelikortit[i].selka = 0;
                PAIVITA_KORTTI(i);
                toista_aani(AANI_KORTTI_KAANTYY);
            }
        }
        return;
    }

    if (jaa_ctr) {
        jaa_ctr -= (tickskip + 1);
        if (jaa_ctr >= 6 - tickskip && jaa_ctr <= 6) {
            jaa_yksi_kortti(jaa_i, 1);
            toista_aani(AANI_KORTTI_JAETAAN);
        }
        if (jaa_ctr <= 0) {
            jaa_ctr = 0;
            ++jaa_i;
        }
    } else {
        int x2 = POYTA_X(jaa_i);
        pelikortit[0].x = x = MIN(pelikortit[0].x + SPEED_MUL(16), x2);
        if (x == x2)
            jaa_ctr = 10;
    }
    PAIVITA_KORTTI(0);
}

void jaa_tuplaus_kortti(void) {
    if (!jaa_i) {
        ++jaa_i;
        tuplaus_kortti = jaa_yksi_kortti(3, 1);
    }
    if (ANIM_LE(16)) {
        pelikortit[3].x = lerp(PAKKA_X, POYTA_X(3), 0, anim, 16);
        pelikortit[3].y = lerp(PAKKA_Y, TUPLAUS_Y, 0, anim, 16);
        PAIVITA_KORTTI(0);
        PAIVITA_KORTTI(3);
    }
    if (ANIM_EQ(16))
        alusta_tila(T_TUPLAUS);
}

void update_voitto_speed(int offset, char slow) {
    offset *= tickskip + 1;
    if (slow) offset >>= 2;
    if (offset > 400)
        voitto_speed = 8, voitto_speed_mul = 4;
    else if (offset > 200)
        voitto_speed = 4, voitto_speed_mul = 4;
    else if (offset > 100)
        voitto_speed = 2, voitto_speed_mul = 4;
    else if (offset > 50)
        voitto_speed = 2, voitto_speed_mul = 2;
    else if (offset > 25)
        voitto_speed = 2, voitto_speed_mul = 1;
    else if (offset > 15)
        voitto_speed = 4, voitto_speed_mul = 1;
    else if (offset > 10)
        voitto_speed = 6, voitto_speed_mul = 1;
    else if (offset > 5)
        voitto_speed = 10, voitto_speed_mul = 1;
    else
        voitto_speed = 16, voitto_speed_mul = 1;
}

char valinnat[5] = { 0 };
char pudotettavat = 0;
char* valintavalot[5] = { &valot.valitse_1, &valot.valitse_2,
        &valot.valitse_3, &valot.valitse_4, &valot.valitse_5 };
char valintanakyy[5] = { 0, 0, 0, 0, 0 };
int valintoja_nakyvissa = 0;
signed char valintaajastin[5] = { 0, 0, 0, 0, 0 };

void arvioi_kasi(void) {
    fast_memset(valinnat, 0, sizeof(valinnat));
    pelikortit[0].x = PAKKA_X;
    pelikortit[0].y = PAKKA_Y;
    card0zoom = 0;
    PYSAYTA_AANET();
    PAIVITA_KORTTI(0);
    if (arvioi_voitto_kasi()) {
        voitto = panos * (jokeri ?
            kadet[voitto_kasi].kerroin_jokeri : kadet[voitto_kasi].kerroin);
        if (voitto > paavoitto) voitto = paavoitto;
        update_voitto_speed(voitto, 0);
        alusta_tila(T_VOITTO);
    } else {
        voitto = 0;
        alusta_tila(T_PANOS);
    }
    voitto_disp = 0;
}

void pudota_kortit(void) {
    int i;

    if (!jaa_ctr) {
        while (!(pudotettavat & (1 << (jaa_i - 1))) && jaa_i <= 5) {
            ++jaa_i;
        }
        if (jaa_i > 5) {
            alusta_tila(T_UJAKO2);
            return;
        }
        jaa_ctr = 1;
    }

    i = jaa_i;
    pelikortit[i].y += SPEED_MUL(12);
    pelikortit[i].selka = 1;
    if (pelikortit[i].y >= ALAPALKKI_Y) {
        pelikortit[i].nakyva = 0;
        ++jaa_i;
        jaa_ctr = 0;
    }
    PAIVITA_KORTTI(i);
}

int jaa_n = 1;
void jaa_uudet_kortit(void) {
    int i, x, wait = 0;

    for (i = 1; i <= 5; ++i) {
        if (pelikortit[i].nakyva && pelikortit[i].selka
                && pelikortit[i].y < POYTA_Y) {
            pelikortit[i].y = MIN(pelikortit[i].y + SPEED_MUL(16), POYTA_Y);
            if (pelikortit[i].y == POYTA_Y) {
                pelikortit[i].selka = 0;
                toista_aani(AANI_UUSI_KORTTI_ASETTUU);
            } else
                wait = 1;
            PAIVITA_KORTTI(i);
        }
    }

    if (wait) {
        PAIVITA_KORTTI(0);
        return;
    }

    if (jaa_ctr) {
        if (!pudotettavat) {
            pelikortit[0].y = MIN(pelikortit[0].y + SPEED_MUL(8), POYTA_Y);
            if (pelikortit[0].y == POYTA_Y) {
                jaa_yksi_kortti(jaa_i, 0);
                alusta_tila(T_UJAKO3);
            }
            PAIVITA_KORTTI(0);
            return;
        }
        jaa_ctr -= SPEED_MUL(1);
        if (jaa_ctr <= 0) {
            jaa_ctr = 0;
            jaa_yksi_kortti(jaa_i, 1);
            toista_aani(AANI_KORTTI_JAETAAN);
            PAIVITA_KORTTI(jaa_i);
            jaa_n = 1;
            ++jaa_i;
        }
    } else {
        int x2;
        if (jaa_n) {
            while (!(pudotettavat & 1)) {
                ++jaa_i;
                pudotettavat >>= 1;
            }
            pudotettavat >>= 1;
            jaa_n = 0;
        }
        x2 = POYTA_X(jaa_i);
        pelikortit[0].x = x = MIN(pelikortit[0].x + SPEED_MUL(16), x2);
        if (x == x2) {
            jaa_ctr = 5;
            if (!pudotettavat)
                toista_aani(AANI_KORTTI_JAETAAN);
        }
    }
    PAIVITA_KORTTI(0);
}

void valitse_kortti(int i) {
    --i;
    if (valinnat[i] ^= 1) {
        VALO_PAALLE(*valintavalot[i]);
        if (!valintanakyy[i]) {
            valintanakyy[i] = 1;
            ++valintoja_nakyvissa;
            valintaajastin[i] = -32;
        }
    } else {
        VALO_VILKKU(*valintavalot[i], 1);
    }
    paivita_palkki();
    toista_aani(AANI_VALINTA);
}

void paivita_valinnat(void) {
    char i, wn;
    for (i = 0; i < 5; ++i) {
        if (valintanakyy[i] && (!valinnat[i] || valintaajastin[i] != 0)) {
            wn = valintaajastin[i] < 0;
            if (valintaajastin[i] >= 32) {
                if (valinnat[i])
                    valintaajastin[i] = -32;
                else {
                    valintanakyy[i] = 0;
                    --valintoja_nakyvissa;
                }
            }        
            valintaajastin[i] += SPEED_MUL(1);
            if (wn && valinnat[i] && valintaajastin[i] > 0)
                valintaajastin[i] = 0;
            paivita_alapalkki_valinnat();
        }
    }
}

void piirra_valinnat(char paivita) {
    short i, y = GAREA_HEIGHT - 32;
    for (i = 0; i < 5; ++i) {
        if (valintanakyy[i]) {
            piirra_kuva_vierita(POYTA_X(i + 1), y, 96, 32,
                    valittu_kuva, y, GAREA_HEIGHT, valintaajastin[i]);
            if (paivita)
                paivita_alue(POYTA_X(i + 1), y, 96, 32);
        }
    }
}

void piirra_alapalkki_vain_voitto(char paivita) {
    piirra_suorakulmio(264, ALAPALKKI_Y + 8, 112, 16, 1);
    piirra_levea_luku_oikea(344, ALAPALKKI_Y + 10, 15, voitto_disp, 0);
    if (paivita)
        paivita_alue(264, ALAPALKKI_Y + 8, 112, 16);
}

void piirra_alapalkki_voitto(char paivita) {
    const int y = ALAPALKKI_Y;
    switch (tila)
    {
    case T_VOITTO:
    case T_TUPLAOK:
        if (english)
            piirra_teksti_oikea(160, y + 10, 11, 0, "YOU WON", 0);
        else
            piirra_teksti(160, y + 10, 11, 0, "PANOS", 0);
        piirra_teksti(424, y + 10, 11, 0,
                    english ? "WANT TO DOUBLE?" : "TUPLAATKO", 0);
        piirra_kuva_maski(256, y + 8, 8, 16, G_tulosv, G_tulosv_M);
        piirra_kuva_maski(376, y + 8, 8, 16, G_tuloso, G_tuloso_M);
        break;
    case T_TUPLA1:
    case T_TUPLA2:
    case T_TUPLA3:
    case T_TUPLAEI:
    case T_TUPLAUS:
        piirra_teksti(424, y + 10, 11, 0,
                english ? "WANT TO DOUBLE?" : "TUPLAATKO", 0);
    case T_TUPLA4:
        piirra_teksti(160, y + 10, 11, 0, english ? "BET" : "PANOS", 0);
        piirra_kuva_maski(256, y + 8, 8, 16, G_tulosv, G_tulosv_M);
        piirra_kuva_maski(376, y + 8, 8, 16, G_tuloso, G_tuloso_M);
        break;
    }
    piirra_alapalkki_vain_voitto(0);
    if (paivita)
        paivita_alue(0, y, GAREA_WIDTH, 32);
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

    tuplaus_banner_x = (tuplaus_banner_x + SPEED_MUL(1)) % TUPLAUS_WIDTH;
    paivita_alue(416, y, 128, 16);
}

void draw_konkka_line(short y) {
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

void lopeta_esittely(void) {
    char i;
    PYSAYTA_AANET();
    valmistele_ruutu_peli();
    alusta_tila(T_PANOS);
    pelikortit[0].nakyva = 1;
    pelikortit[0].selka = 1;
    pelikortit[0].x = PAKKA_X;
    pelikortit[0].y = PAKKA_Y;
    for (i = 1; i <= 5; ++i) {
        pelikortit[i].nakyva = 0;
    }
    if (!napit.panos && !napit.jako)
        toista_aani(AANI_ALOITA);
    paivita_kaikki_kortit();
    skip_input_read = 1;
}

void esittely1(void) {
    char i, any_card_visible = 0, stop;
    if (napit.mika_tahansa) {
        lopeta_esittely();
        return;
    }
    present_y += SPEED_MUL(1);
    stop = present_y >= 400 && present_y < 400 + 300;
    if (stop)
        any_card_visible = 1;
    else {
        for (i = 1; i <= 5; ++i) {
            if (pelikortit[i].nakyva) {
                short x = pelikortit[i].x, oy = pelikortit[i].y, ny;
                any_card_visible = 1;
                pelikortit[i].y = ny = oy + SPEED_MUL(1);
                if (ny > -KORTTI_K || oy > -KORTTI_K) {
                    short py, ty, kh;
                    if (ny < 0)
                        py = 0, ty = -ny;
                    else
                        py = ny, ty = 0;
                    if (ny > GAREA_HEIGHT)
                        ny = GAREA_HEIGHT;
                    if (ny >= 0) {
                        short fy = oy;
                        if (oy < 0) fy = 0;
                        piirra_suorakulmio(x, fy, KORTTI_L, ny - oy, 0);
                    }
                    piirra_kuva_rajaa_y(x, py, KORTTI_L, KORTTI_K - ty,
                        KORTTI_K, kortti_caches[i] + ty * (KORTTI_L / PPB),
                        GAREA_HEIGHT);
                    py = oy;
                    if (py < 0)
                        py = 0;
                    kh = KORTTI_K - ty + (ny - oy);
                    if (py + kh > GAREA_HEIGHT)
                        kh = GAREA_HEIGHT - py;
                    if (kh > 0)
                        paivita_alue(x, py, KORTTI_L, kh);
                }
                if (ny == GAREA_HEIGHT) {
                    pelikortit[i].nakyva = 0;
                }
            }
        }
    }
    if (!any_card_visible) {
        alusta_pakka();
        for (i = 52; i > 0; --i)
            sekoita_pakka(i);
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 1;
            pelikortit[i].kortti = jaa_kortti();
            pelikortit[i].y = -KORTTI_L - (4 + (rand() % 8)) * 16;
            piirra_kortti(0, 0, pelikortit[i].kortti);
            sailyta_kuva(0, 0, KORTTI_L, KORTTI_K, kortti_caches[i]);
        }
        piirra_suorakulmio(0, 0, KORTTI_L, KORTTI_K, 0);
        present_y = 0;
        if (!inactive--)
            alusta_tila(T_ESIT2);
    }
}

void esittely2(void) {
    char i, any_card_visible = 0, stop;
    if (napit.mika_tahansa) {
        lopeta_esittely();
        return;
    }
    if (present_y < PAKKA_Y) {
        short oy = present_y, ny;
        present_y += SPEED_MUL(1);
        if (present_y >= PAKKA_Y) {
            present_y = PAKKA_Y;
        }
        ny = present_y;
        for (i = 1; i <= 5; ++i) {
            if (pelikortit[i].nakyva) {
                short x = pelikortit[i].x;
                if (ny > -KORTTI_K || oy > -KORTTI_K) {
                    short py, ty, kh;
                    if (ny < 0)
                        py = 0, ty = -ny;
                    else
                        py = ny, ty = 0;
                    if (ny >= 0) {
                        short fy = oy;
                        if (oy < 0) fy = 0;
                        piirra_suorakulmio(x, fy, KORTTI_L, ny - oy, 0);
                    }
                    piirra_kuva_rajaa_y(x, py, KORTTI_L, KORTTI_K - ty,
                        KORTTI_K, kortti_caches[i] + ty * (KORTTI_L / PPB),
                        GAREA_HEIGHT);
                    py = oy;
                    if (py < 0)
                        py = 0;
                    kh = KORTTI_K - ty + (ny - oy);
                    if (py + kh > GAREA_HEIGHT)
                        kh = GAREA_HEIGHT - py;
                    if (kh > 0)
                        paivita_alue(x, py, KORTTI_L, kh);
                }
            }
        }
        if (present_y == PAKKA_Y) {
            inactive = 720;
            pelikortit[0].x = -KORTTI_L * 2;
            piirra_selka(0, CARDAREA_HEIGHT - KORTTI_K);
            sailyta_kuva(0, CARDAREA_HEIGHT - KORTTI_K,
                                KORTTI_L, KORTTI_K, kortti_cache);
            piirra_suorakulmio(0, CARDAREA_HEIGHT - KORTTI_K,
                                KORTTI_L, KORTTI_K, 0);
            present_panos = 0;
        }
    }
    if (present_y == PAKKA_Y) {
        if (!inactive) {
            short ox = pelikortit[0].x, x = ox, x1, x2, w;
            if (x >= GAREA_WIDTH * 2 || tickskip >= 4) {
                present_hand = (present_hand + 1) % KASI_LKM;
                alusta_tila(rand() & 1 ? T_ESIT0 : T_ESIT1);
                return;
            }
            x += SPEED_MUL(8);
            pelikortit[0].x = x;
            if (x > GAREA_WIDTH) {
                return;
            }
            w = KORTTI_L;
            if (x < 0)
                x1 = 0, x2 = -x, w += x;
            else
                x1 = x, x2 = 0;
            if (x + w > GAREA_WIDTH)
                w = GAREA_WIDTH - x;
            if (ox >= 0) {
                piirra_suorakulmio(ox, PAKKA_Y, x - ox, KORTTI_K, 0);
            }
            if (w > 0) {
                piirra_kuva_rajaa(x1, PAKKA_Y, w, KORTTI_K,
                            x2, 0, KORTTI_L, KORTTI_K,
                            kortti_cache);
                if (ox >= 0)
                    paivita_alue(ox, PAKKA_Y, w + (x - ox), KORTTI_K);
                else
                    paivita_alue(0, PAKKA_Y, w, KORTTI_K);
            } else if (x >= GAREA_WIDTH) {
                paivita_alue(ox, PAKKA_Y, x - ox, KORTTI_K);
            }
        } else {
            if (inactive >= 660 && inactive <= 660 + tickskip) {
                piirra_teksti_keski(540, 260, 14, 1,
                            english ? "BET" : "PANOS", 1);
                piirra_teksti_oikea(560, 300, 14, 0, "@", 1);
            }
            if (present_panos < maksimipanos && inactive <= 660 + tickskip
                    && (inactive - 60) % 120 <= tickskip) {
                short x = POYTA_X(1), y = PAKKA_Y + KORTTI_K + 64, kerroin;
                ++present_panos;
                piirra_suorakulmio(500, 300, 40, 12, 0);
                piirra_luku_oikea(540, 300, 14, 1, present_panos, 1);
                paivita_alue(x, y, 400, 12 * KASI_LKM);
                for (i = 0; i < KASI_LKM; ++i) {
                    piirra_suorakulmio(x, y, 400, 12,
                        i == present_hand ? 2 : 0);
                    piirra_teksti(x, y, 14,
                        i == present_hand, kadet[i].nimi, 0);
                    piirra_teksti_oikea(x + 400, y, 14, 0, "@", 0);
                    kerroin = (present_panos >= jokeri_minimipanos
                                ? kadet[i].kerroin_jokeri 
                                : kadet[i].kerroin);
                    if (kerroin)
                        piirra_luku_oikea(x + 384, y, 14, 1, 
                            present_panos * kerroin, 0);
                    else
                        piirra_teksti_oikea(x + 384, y, 14, 1, "-", 0);
                    y += 12;
                }
            }
            inactive -= SPEED_MUL(1);
            if (inactive < 0)
                inactive = 0;
            if (!inactive) {
                piirra_suorakulmio(0, PAKKA_Y + KORTTI_K + 64,
                        GAREA_WIDTH, 128, 0);
                paivita_alue(0, PAKKA_Y + KORTTI_K + 64,
                        GAREA_WIDTH, 128);
            }
        }
    }
}

int u4x = 0, u4y = 0;
long ufrac = 0;

void alusta_tila(enum Pelitila t) {
    switch (t) {
    case T_PANOS: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.panos, 1);
        if (!pelit && !voitot && !voitto) {
            alusta_tila(T_KONKKA);
            toista_musiikki_oletus(MUSA_KONKKA);
            return;
        } else if (panos > pelit)
            panos = pelit;
        VALO_VILKKU(valot.jako, 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        piirra_kadet(0);
        inactive = 0;
    } break;
    case T_VSIIRTO: {
        oli_paavoitto = voitto * 2 >= paavoitto;
        voitto_disp = voitto;
        palswap = 0;
        update_voitto_speed(voitto, oli_paavoitto);
        if (tila == T_VOITTO)
            piirra_kadet(1);
    } break;
    case T_JAKO1: {
        int i, j = 0;
        if (alapalkki_fade)
            alapalkki_fade = 9;
        if (voitot > 0) {
            pelit += voitot;
            voitot = 0;
            paivita_ylapalkki();
        }
        for (i = 1; i <= 5; ++i)
            pelikortit[i].selka = 1, j |= pelikortit[i].nakyva;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        PYSAYTA_AANET();
        paivita_kaikki_kortit();
        card0zoom = 1;
    } break;
    case T_JAKO2: {
        tuplaus_ctr = 0;
        tuplaus = 0;
        card0zoom = 1;
        shuffle_i = jokeri ? 52 : 51;
        p0 = p1 = p2 = p0d = p1d = p2d = 0;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        toista_aani(AANI_SEKOITA);
    } break;
    case T_JAKO3: {
        pelikortit[0].x = PAKKA_X, pelikortit[0].y = PAKKA_Y;
        jaa_i = 1, jaa_ctr = 0;
    } break;
    case T_VALITSE: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.valitse_1, 1);
        VALO_VILKKU(valot.valitse_2, 1);
        VALO_VILKKU(valot.valitse_3, 1);
        VALO_VILKKU(valot.valitse_4, 1);
        VALO_VILKKU(valot.valitse_5, 1);
        VALO_VILKKU(valot.jako, 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        fast_memset(valinnat, 0, sizeof(valinnat));
        piirra_kadet(0);
    } break;
    case T_UJAKO1: {
        int i;
        kaikki_valot_pois();
        paivita_palkki();
        pudotettavat = 0;
        jaa_i = 1, jaa_ctr = 0;
        for (i = 0; i < 5; ++i) {
            pudotettavat |= (~valinnat[i] & 1) << i;
            if (!valinnat[i])
                pelikortit[i + 1].selka = 1;
        }
        paivita_kaikki_kortit();
        if (!pudotettavat) {
            arvioi_kasi();
            return;
        } else
            toista_aani(AANI_KORTTIEN_PUDOTUS);
    } break;
    case T_UJAKO2: {
        tuplaus = 0;
        pelikortit[0].x = PAKKA_X, pelikortit[0].y = PAKKA_Y;
        jaa_i = 1, jaa_ctr = 0, jaa_n = 1;
    } break;
    case T_UJAKO3: {
        tuplaus = 0;
        card0zoom = 1;
        sailotty_miny = POYTA_Y;
        sailotty_maxy = POYTA_Y + 8;
        kortti_sailotty = 0;
        card_fall_speed = 4096 / (tickskip + 1);
        sailotty_kortti = jaa_i;
        toista_musiikki_oletus(MUSA_KOPUTUS);
        fast_memset(valinnat, 0, sizeof(valinnat));
    } break;
    case T_UJAKO4: {
        u4x = pelikortit[0].x;
        u4y = pelikortit[0].y;
        ufrac = 0;
        PYSAYTA_AANET();
    } break;
    case T_TUPLA1: {
        int i, j = 0;
        for (i = 1; i <= 5; ++i)
            pelikortit[i].selka = 1, j |= pelikortit[i].nakyva;
        voitto_kasi = -1;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        piilota_kadet();
        paivita_kaikki_kortit();
        paivita_alapalkki();
        card0zoom = 1;
    } break;
    case T_TUPLA2: {
        voitto *= 2;
        update_voitto_speed(voitto, 0);
    } break;
    case T_TUPLA3: {
        card0zoom = 1;
        shuffle_i = jokeri ? 52 : 51;
        p0 = p1 = p2 = p0d = p1d = p2d = 0;
        pelikortit[0].x = PAKKA_X;
        pelikortit[0].y = PAKKA_Y;
        pelikortit[1].nakyva = 0;
        pelikortit[2].nakyva = 0;
        pelikortit[4].nakyva = 0;
        pelikortit[5].nakyva = 0;
        toista_aani(AANI_SEKOITA);
    } break;
    case T_TUPLA4: {
        card0zoom = 1;
        tuplaus = 1;
        pelikortit[0].x = PAKKA_X, pelikortit[0].y = PAKKA_Y;
        jaa_i = 0;
        tuplaus_banner_x = 50;
        kaikki_valot_pois();
        paivita_alapalkki();
        VALO_VILKKU(valot.tuplaus_pieni, 1);
        VALO_VILKKU(valot.tuplaus_suuri, 2);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        toista_musiikki(MUSA_TUPLAUS, 120 + tuplaus_ctr * 24);
    } break;
    case T_TUPLAUS:
        card0zoom = 0;
        break;
    case T_TUPLAOK: {
        kaikki_valot_pois();
        VALO_VILKKU(valot.voitot, 1);
        if (voitto * 2 <= paavoitto)
            VALO_VILKKU(valot.tuplaus, 1);
        else
            piirra_teksti_keski(320, 144, 15, 0,
                    (english ? tuplaus_onnittelut_e : tuplaus_onnittelut)
                    [rand() % ONNITTELU_N], 1);
        VALO_VILKKU_ALUSTA();
        paivita_palkki();
        paivita_alapalkki();
        ++tuplaus_ctr;
        toista_musiikki(MUSA_VOITTO, 96 + tuplaus_ctr * 24);
    } break;
    case T_TUPLAEI: {
        toista_musiikki_oletus(AANI_TUPLAUS_HAVITTY);
        voitto = voitto_disp = 0;
        paivita_alapalkki();
        kaikki_valot_pois();
        paivita_palkki();
    } break;
    case T_VOITTO: {
        kaikki_valot_pois();
        anim = 0;
        piirra_kadet(0);
        paivita_palkki();
        fast_memset(valinnat, 0, sizeof(valinnat));
        musan_toisto = 0;
        voitto_vilkku = 0;
        if (tila != T_VOITTO)
            alapalkki_fade = 1;
    } break;
    case T_ESIT0: {
        char i, y, x;
        const char *p = logokartta;
        valmistele_ruutu_esittely();
        kortti_caches[1] = kortti_cache;
        kortti_caches[2] = kortti_cache1;
        kortti_caches[3] = kortti_cache2;
        kortti_caches[4] = kortti_cache3;
        kortti_caches[5] = kortti_cache4;
        alapalkki_fade = 0;
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 0;
            pelikortit[i].selka = 0;
            pelikortit[i].x = POYTA_X(i);
        }
        for (i = 0; i < 25; ++i) {
            piirra_selka((rand() % (GAREA_WIDTH - KORTTI_L - 64) + 32) & ~7,
                        (rand() % (GAREA_HEIGHT - KORTTI_K - 64) + 32) & ~7);
        }
        piirra_teksti(GAREA_WIDTH + 16, 16,
            15, 1, "VIDEOPOKERI-DOS/386", 0);
        piirra_teksti_oikea(GAREA_WIDTH - 16, GAREA_HEIGHT - 32,
            15, 1, "ORIGINAL 1986 RAY", 0);
        piirra_teksti_oikea(GAREA_WIDTH - 16, GAREA_HEIGHT - 16,
            15, 1, "DOS 2021 RISTIJŽTKŽ", 0);
        for (y = 0; y < 12; ++y) {
            for (x = 0; x < 36; ++x) {
                if (*p++ != ' ')
                    piirra_suorakulmio(32 + x * 16, 80 + y * 16, 16, 16, 10);
            }
        }
        paivita_alue(0, 0, GAREA_WIDTH, GAREA_HEIGHT);
        inactive = 600;
    } break;
    case T_ESIT1: {
        char i;
        valmistele_ruutu_esittely();
        present_y = 0;
        inactive = 1 + (rand() % 4);
        for (i = 1; i <= 5; ++i) {
            pelikortit[i].nakyva = 0;
            pelikortit[i].selka = 0;
            pelikortit[i].x = POYTA_X(i);
        }
    } break;
    case T_ESIT2: {
        char i;
        kortti_t k;
        valmistele_ruutu_esittely();
        present_y = -KORTTI_K - 32;
        inactive = 1;
        for (i = 1; i <= 5; ++i) {
            k = kadet_esittely[present_hand].kortit[i - 1];
            if (k == K_SELKA) {
                piirra_selka(0, 0);
            } else {
                piirra_kortti(0, 0, k);
            }
            sailyta_kuva(0, 0, KORTTI_L, KORTTI_K, kortti_caches[i]);
        }
        piirra_suorakulmio(0, 0, KORTTI_L, KORTTI_K, 0);
    } break;
    case T_KONKKA: {
        assert(!voitot && !voitto);
        jaa_i = 0;
        kaikki_valot_pois();
        piirra_suorakulmio(240, 160, 160, 32, 5);
        piirra_teksti_keski(320, 170, 15, 1, "GAME OVER", 0);
        paivita_alue(240, 160, 160, 32);
    } break;
    }
    tila = t;
    anim = 0;
    new_mode = 1;
}

void aja_peli_internal(void) {
    switch (tila) {
    case T_PANOS: {
        if (napit.mika_tahansa)
            inactive = 0;
        else
            inactive += SPEED_MUL(1);
        if (napit.panos) {
            panos = (panos % maksimipanos) + 1;
            if (panos > pelit + voitot)
                panos = pelit + voitot;
            toista_aani(AANI_PANOS1 + (panos - 1));
            jokeri = panos >= jokeri_minimipanos;
            paivita_ylapalkki();
            piirra_kadet(0);
            VALO_VILKKU(valot.jako, 1);
        }
        if (pelit + voitot >= panos && napit.jako) {
            kaikki_valot_pois();
            VALO_PAALLE(valot.jako);
            paivita_palkki();
            piilota_kadet();
            pelit = pelit - panos + voitot;
            voitot = 0;
            paivita_ylapalkki();
            paivita_alapalkki();
            alusta_pakka();
            alusta_tila(pelikortit[3].nakyva ? T_JAKO1 : T_JAKO2);
        }
        if (inactive >= 1800) {
            kaikki_valot_pois();
            valot_efekti();
            alusta_tila(T_ESIT0);
        }
        break;
    }
    case T_JAKO1: /* ker„„ kortit */
        keraa_kortit(T_JAKO2);
        break;
    case T_JAKO2: /* sekoita */
        pakan_sekoitus(T_JAKO3);
        break;
    case T_JAKO3: /* jaa kortit */
        jaa_kortit();
        break;
    case T_TUPLA1: /* ker„„ kortit */
        keraa_kortit(T_TUPLA2);
        break;
    case T_TUPLA2: /* panos tuplataan */
        if (voitto_disp < voitto && (anim % voitto_speed) <= tickskip) {
            voitto_disp += voitto_speed_mul;
            if (voitto_disp > voitto)
                voitto_disp = voitto;
            toista_aani(AANI_VOITONMAKSU);
            paivita_alapalkki_voitto();
        } else if (voitto_disp == voitto)
            alusta_tila(T_TUPLA3);
        break;
    case T_TUPLA3: /* sekoita */
        pakan_sekoitus(T_TUPLA4);
        break;
    case T_TUPLA4: /* jaa kortti */
        jaa_tuplaus_kortti();
        paivita_tuplaus_alapalkki();
        break;
    case T_TUPLAUS: { /* arvaa */
        int paina = 0, oikein = 0;
        if (napit.tuplaus_pieni)
            paina = 1, oikein = KARVO(tuplaus_kortti) < 6;
        if (napit.tuplaus_suuri)
            paina = 1, oikein = KARVO(tuplaus_kortti) > 6;
        if (paina) {
            oikein |= tuplaus_kortti == K_JOKERI;
            pelikortit[3].selka = 0;
            PAIVITA_KORTTI(3);
            alusta_tila(oikein ? T_TUPLAOK : T_TUPLAEI);
        }
        paivita_tuplaus_alapalkki();
        break;
    }
    case T_TUPLAEI:
        if (anim >= 10) {
            if (alapalkki_fade == 8)
                alapalkki_fade = 9;
            else if (!alapalkki_fade)
                alusta_tila(pelit ? T_PANOS : T_KONKKA);
        }
        break;
    case T_VALITSE: /* valitse kortit */
        if (napit.valitse_1) valitse_kortti(1);
        if (napit.valitse_2) valitse_kortti(2);
        if (napit.valitse_3) valitse_kortti(3);
        if (napit.valitse_4) valitse_kortti(4);
        if (napit.valitse_5) valitse_kortti(5);
        if (napit.jako) {
            paivita_palkki();
            piilota_kadet();
            paivita_alapalkki();
            alusta_tila(T_UJAKO1);
        }
        break;
    case T_UJAKO1: /* pudota kortit */
        pudota_kortit();
        paivita_alapalkki();
        break;
    case T_UJAKO2: /* jaa uudet */
        jaa_uudet_kortit();
        break;
    case T_UJAKO3: { /* tinnitys j„„vistyy */
        int a = anim / 2, y = pelikortit[0].y;
        static div_t ufrac_div;
        ufrac += a * a + (anim >= 120 ? (anim - 120) << 7 : 1);
        ufrac_div = div(ufrac, card_fall_speed);
        pelikortit[0].y += ufrac_div.quot;
        ufrac = ufrac_div.rem;
        if (y != pelikortit[0].y) {
            PAIVITA_KORTTI(jaa_i);
            PAIVITA_KORTTI(0);
            sailotty_miny = y;
            sailotty_maxy = pelikortit[0].y + 8;
            if (pelikortit[0].y >= GAREA_HEIGHT) {
                kortti_sailotty = 0;
                sailotty_miny = 0;
                sailotty_kortti = 0;
                sailotty_maxy = CARDAREA_HEIGHT;
                if (tickskip > 2) /* „l„ ved„ UJAKO4:sta hitailla koneilla */
                    arvioi_kasi();
                else
                    alusta_tila(T_UJAKO4);
            }
        }
        break;
    }
    case T_UJAKO4: { /* pakka lent„„ */
        int x = pelikortit[0].x, y = pelikortit[0].y;
        if (ANIM_LE(8)) {
            pelikortit[0].nakyva = 1;
            pelikortit[0].x = lerp(u4x, PAKKA_X, 0, anim, 8);
            pelikortit[0].y = lerp(u4y, PAKKA_Y, 0, anim, 8);
        }
        PAIVITA_KORTTI(0);
        if (x < POYTA_X(1) + KORTTI_L)
            PAIVITA_KORTTI(1);
        if (x >= POYTA_X(2) - KORTTI_L && x < POYTA_X(2) + KORTTI_L)
            PAIVITA_KORTTI(2);
        if (x >= POYTA_X(3) - KORTTI_L && x < POYTA_X(3) + KORTTI_L)
            PAIVITA_KORTTI(3);
        if (x >= POYTA_X(4) - KORTTI_L && x < POYTA_X(4) + KORTTI_L)
            PAIVITA_KORTTI(4);
        if (x >= POYTA_X(5) - KORTTI_L && x < POYTA_X(5) + KORTTI_L)
            PAIVITA_KORTTI(5);
        if (anim >= 9 + tickskip)
            arvioi_kasi();
        break;
    }
    case T_VOITTO: {
        if (!voitto_vilkku) {
            VALO_VILKKU(valot.voitot, 1);
            if (voitto * 2 <= paavoitto)
                VALO_VILKKU(valot.tuplaus, 1);
            VALO_VILKKU_ALUSTA();
            voitto_vilkku = 1;
            anim = 0;
            paivita_palkki();
        } else if ((anim & 31) <= tickskip) {
            vilkuta_voittokatta();
        }
        if (napit.voitot || voitto * 2 > paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_VSIIRTO);
        }
        if (napit.tuplaus && voitto * 2 <= paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_TUPLA1);
        }
        if (voitto_disp < voitto && (anim % voitto_speed) <= tickskip) {
            voitto_disp += voitto_speed_mul;
            if (voitto_disp > voitto)
                voitto_disp = voitto;
            toista_aani(AANI_VOITONMAKSU);
            paivita_alapalkki_voitto();
        } else if (voitto_disp == voitto && !musan_toisto) {
            toista_musiikki(MUSA_VOITTO, 96 + tuplaus_ctr * 24);
            musan_toisto = 1;
        }
        break;
    }
    case T_TUPLAOK: {
        if (napit.voitot || voitto * 2 > paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_VSIIRTO);
        }
        if (napit.tuplaus && voitto * 2 <= paavoitto) {
            kaikki_valot_pois();
            paivita_palkki();
            PYSAYTA_AANET();
            alusta_tila(T_TUPLA1);
        }
        break;
    }
    case T_VSIIRTO: {
        if (oli_paavoitto && anim % 8 <= tickskip) {
            if (palswap)
                vaihda_normaaliin_palettiin();
            else
                vaihda_liilaan_palettiin();
            palswap ^= 1;
        }
        if (anim % voitto_speed > tickskip) return;
        if (!voitto) {
            if (alapalkki_fade)
                alapalkki_fade = 9;
            if (oli_paavoitto) {
                piirra_suorakulmio(160, 144, 480, 16, 0);
                paivita_alue(160, 144, 480, 16);
                palswap = 0;
                vaihda_normaaliin_palettiin();
                paivita_kaikki_kortit();
                toista_musiikki_oletus(MUSA_PAAVOITTO);
            }
            oli_paavoitto = 0;
            paivita_ylapalkki();
            paivita_alapalkki();
            voitto_kasi = -1;
            piirra_kadet(0);
            alusta_tila(T_PANOS);
        } else {
            short maxv = voitto_speed_mul;
            if (maxv > voitto) maxv = voitto;
            voitot += maxv;
            voitto -= maxv;
            voitto_disp = voitto;
            toista_aani(AANI_VOITONMAKSU);
            paivita_ylapalkki_voitot();
            paivita_alapalkki_voitto();
        }
        break;
    }
    case T_KONKKA: {
        short i, y;
        if (anim >= 300)
            jatka = 0;
        if (jaa_i < GAREA_HEIGHT / 2) {
            for (i = jaa_i; i < anim; ++i) {
                if (i >= GAREA_HEIGHT / 2) continue;
                y = i >= 100;
                draw_konkka_line(y + (i % 100) * 4);
                draw_konkka_line(y + (i % 100) * 4 + 2);
            }
        }
        jaa_i = anim;
        break;
    }
    case T_ESIT0: {
        if (napit.mika_tahansa) {
            lopeta_esittely();
            return;
        }
        if (inactive <= 0)
            alusta_tila(T_ESIT1);
        else
            inactive -= SPEED_MUL(1);
        break;
    }
    case T_ESIT1:
        esittely1();
        break;
    case T_ESIT2:
        esittely2();
        break;
    }
}

void aja_peli(void) {
    ++tick;
    if (tickskip && tickskip_i++ < tickskip)
        return;
    tickskip_i = 0;
    if (tila == T_VOITTO && valintoja_nakyvissa) {
        paivita_valinnat();
        return;
    }
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
    if (napit.lopeta) {
        pelit += voitot + voitto;
        jatka = 0;
        return;
    }
    aja_peli_internal();
    paivita_valinnat();
    paivita_kortit();
    if (!new_mode)
        anim += SPEED_MUL(1);
    new_mode = 0;
}
