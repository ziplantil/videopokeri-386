
#include <string.h>

#include "POKERI.H"
#include "POYTA.H"
#include "KORTTI.H"
#include "MUISTI.H"

int maksimipanos = 5;
int paavoitto = 100;
int jokeri_saatavilla = 1;
int jokeri_minimipanos = 3;

/* nimi, kerroin ilman jokeria, kerroin jos jokeri */
struct Pokerikasi kadet[KASI_LKM] = {
    { "VIITOSET",       0, 20 },
    { "VÄRISUORA",     20, 20 },
    { "NELOSET",       15, 15 },
    { "TÄYSKÄSI",      10, 10 },
    { "VÄRI",           5,  5 },
    { "SUORA",          4,  4 },
    { "KOLMOSET",       2,  2 },
    { "KAKSI PARIA",    2,  2 }
};

#define MK(m,k) (((m) << 4) + (k))
extern struct Korttikasi kadet_esittely[KASI_LKM] = {
    { { MK(0,  6), MK(3,  6), MK(2,  6), MK(1,  6), K_JOKERI } },
    { { MK(3,  0), MK(3,  1), MK(3,  2), MK(3,  3), MK(3,  4) } },
    { { MK(0, 11), MK(3, 11), MK(2, 11), MK(1, 11), K_SELKA } },
    { { MK(0,  8), MK(2,  8), MK(3,  8), MK(1,  4), MK(2,  4) } },
    { { MK(1,  1), MK(1,  3), MK(1,  6), MK(1,  8), MK(1, 12) } },
    { { MK(2,  3), MK(1,  4), MK(0,  5), MK(3,  6), MK(2,  7) } },
    { { MK(0,  2), MK(3,  2), MK(2,  2), K_SELKA, K_SELKA } },
    { { MK(2,  0), MK(1,  0), MK(0, 12), MK(3, 12), K_SELKA } },
};

int voitto_kasi = -1;
kortti_t poyta[5] = { 0 };
kortti_t tuplakortti = { 0 }; 

void sort_hand(kortti_t *hand, int n, int *flush) {
    int i = 1, j, f = 0xFF;
    kortti_t tmp;
    while (i < n) {
        j = i;
        while (j > 0 && KARVO(hand[j - 1]) > KARVO(hand[j])) {
            ARRAY_SWAP(hand, j - 1, j, tmp);
            --j;
        }
        ++i;
    }
    for (i = 0; i < n; ++i) {
        f &= 1 << KMAA(hand[i]);
        hand[i] = KARVO(hand[i]);
    }
    *flush = f;
}

static int arvot[16] = { 0 };
static int arvot_m[5] = { 0 };

int arvioi_voitto_kasi(void) {
    int i, vari = 0xFF, suora = 0, jokeri = 0;
    memset(arvot, 0, sizeof(arvot));
    memset(arvot_m, 0, sizeof(arvot_m));

    for (i = 0; i < 5; ++i) {
        vari &= poyta[i] == K_JOKERI ? vari : 1 << KMAA(poyta[i]);
        ++arvot[KARVO(poyta[i])];
    }
    jokeri = jokeri_saatavilla && arvot[15];
    arvot[ARVOT_N] = arvot[0];
    for (i = 0; i < ARVOT_N; ++i) {
        ++arvot_m[arvot[i]];
    }

    suora = arvot_m[1] >= (jokeri ? 4 : 5);
    if (suora) {
        int j, skip = jokeri;
        i = 0;
        while (!arvot[i])
            ++i;
        if (i == 0 && (arvot[11] || arvot[12]))           /* A-10 purkka */
            i = 8;                    /* siirrä alku 10:iin jos A ja K/Q */
        for (j = 1; j < 5; ++j) {
            if (++i >= ARVOT_N) {
                suora = skip && j == 4; /* 10 J Q K jokeri */
                break;
            }
            if (!arvot[i]) {
                if (skip) {
                    skip = 0;
                    continue;
                }
                suora = 0;
                break;
            }
        }
    }

    if (jokeri && arvot_m[4])
        i = 0; /* viitoset */

    else if (vari && suora)
        i = 1; /* värisuora */

    else if (arvot_m[4])
        i = 2; /* neloset */
    else if (jokeri && arvot_m[3])
        i = 2; /* neloset */

    else if (arvot_m[3] && arvot_m[2])
        i = 3; /* täyskäsi */
    else if (jokeri && arvot_m[2] == 2)
        i = 3; /* täyskäsi */

    else if (vari)
        i = 4; /* väri */

    else if (suora)
        i = 5; /* suora */

    else if (arvot_m[3])
        i = 6; /* kolmoset */
    else if (jokeri && arvot_m[2])
        i = 6; /* kolmoset */

    else if (arvot_m[2] == 2)
        i = 7; /* kaksi paria */

    else
        i = -1; /* ei mitään */

    voitto_kasi = i;
    return i >= 0;
}
