
#include <string.h>

#include "KORTTI.H"
#include "MUISTI.H"
#include "POKERI.H"
#include "POYTA.H"

int maksimipanos = 5;
int paavoitto = 100;
int jokeri_saatavilla = 1;
int jokeri_minimipanos = 3;

/* nimi, kerroin ilman jokeria, kerroin jos jokeri */
struct Pokerikasi kadet[MAX_KASI_LKM];

#define MK(m,k) (((m) << 4) + (k))
struct Korttikasi kadet_esittely[MAX_KASI_LKM] = {
    { { MK(3,  0), MK(3, 12), MK(3, 11), MK(3, 10), MK(3,  9) } },
    { { MK(0,  6), MK(3,  6), MK(2,  6), MK(1,  6), K_JOKERI } },
    { { MK(3,  0), MK(3,  1), MK(3,  2), MK(3,  3), MK(3,  4) } },
    { { MK(0, 11), MK(3, 11), MK(2, 11), MK(1, 11), K_SELKA } },
    { { MK(0,  8), MK(2,  8), MK(3,  8), MK(1,  4), MK(2,  4) } },
    { { MK(1,  1), MK(1,  3), MK(1,  6), MK(1,  8), MK(1, 12) } },
    { { MK(2,  3), MK(1,  4), MK(0,  5), MK(3,  6), MK(2,  7) } },
    { { MK(0,  2), MK(3,  2), MK(2,  2), K_SELKA, K_SELKA } },
    { { MK(2,  0), MK(1,  0), MK(0, 12), MK(3, 12), K_SELKA } },
    { { MK(1, 10), MK(3, 10), K_SELKA, K_SELKA, K_SELKA } },
};

int voitto_kasi = -1;
kortti_t poyta[5] = { 0 };
kortti_t tuplakortti = { 0 }; 
unsigned kasi_lkm = 0;

static const char *kasi_nimet[MAX_KASI_LKM] = {
    "10-A V.SUORA",
    "VIITOSET",
    "VÄRISUORA",
    "NELOSET",
    "TÄYSKÄSI",
    "VÄRI",
    "SUORA",
    "KOLMOSET",
    "KAKSI PARIA",
    "KUVAPARI",
};

static const char *kasi_nimet_e[MAX_KASI_LKM] = {
    "ROYAL FLUSH",
    "FIVE OF A KIND",
    "STRAIGHT FLUSH",
    "FOUR OF A KIND",
    "FULL HOUSE",
    "FLUSH",
    "STRAIGHT",
    "THREE OF A KIND",
    "TWO PAIRS",
    "JACKS OR BETTER",
};

static int kadet_kaytossa[MAX_KASI_LKM];

int alusta_kadet(int english, const int *kertoimet, const int *kertoimet_j) {
    int i;
    const char **nimet = english ? kasi_nimet_e : kasi_nimet;
    kasi_lkm = 0;
    for (i = 0; i < MAX_KASI_LKM; ++i) {
        int k = kertoimet[i];
        int j = kertoimet_j[i];
        if (k < 0 || j < 0)
            return 1;
        if (k || j) {
            kadet[kasi_lkm].nimi = nimet[i];
            kadet[kasi_lkm].kerroin = k;
            kadet[kasi_lkm].kerroin_jokeri = j;
            kadet[kasi_lkm].esittely = &kadet_esittely[i];
            kadet_kaytossa[i] = kasi_lkm;
            ++kasi_lkm;
        } else {
            kadet_kaytossa[i] = -1;
        }
    }
    return 0;
}

int arvioi_voitto_kasi(void) {
    int i, vari = 0xFF, suora = 0, jokeri = 0, tulos = 0, kasi;
    int arvot[16] = { 0 };
    int arvot_lkm[5] = { 0 };

    for (i = 0; i < 5; ++i) {
        vari &= poyta[i] == K_JOKERI ? vari : 1 << KMAA(poyta[i]);
        ++arvot[KARVO(poyta[i])];
    }
    jokeri = jokeri_saatavilla && arvot[15];
    arvot[ARVOT_N] = arvot[0];
    for (i = 0; i < ARVOT_N; ++i)
        ++arvot_lkm[arvot[i]];

    if ((suora = arvot_lkm[1] == (jokeri ? 4 : 5))) {
        int arvo_min = 0, arvo_max = ARVOT_N - 1;
        for (i = ARVOT_N - 1; i >= 0; --i) {
            if (arvot[i]) {
                arvo_max = i;
                break;
            }
        }
        for (i = 0; i < ARVOT_N; ++i) {
            if (arvot[i]) {
                if (i == 0 && arvo_max == ARVOT_N - 1) {
                    // ässät: korota arvo_maxia (10-J-Q-k-A)
                    arvo_max = ARVOT_N;
                } else {
                    arvo_min = i;
                    break;
                }
            }
        }
        suora = arvo_max - arvo_min <= 4;
    }

    if (vari && suora && arvot[0] && arvot[9])
        tulos |= 1 << KASI_ARVIO_KUNINGASVARISUORA;

    if (jokeri && arvot_lkm[4])
        tulos |= 1 << KASI_ARVIO_VIITOSET;

    if (vari && suora)
        tulos |= 1 << KASI_ARVIO_VARISUORA;

    if (arvot_lkm[jokeri ? 3 : 4])
        tulos |= 1 << KASI_ARVIO_NELOSET;

    if (((arvot_lkm[3] && arvot_lkm[2]) || (jokeri && arvot_lkm[2] == 2)))
        tulos |= 1 << KASI_ARVIO_TAYSKASI;

    if (vari)
        tulos |= 1 << KASI_ARVIO_VARI;

    if (suora)
        tulos |= 1 << KASI_ARVIO_SUORA;

    if (arvot_lkm[jokeri ? 2 : 3])
        tulos |= 1 << KASI_ARVIO_KOLMOSET;

    if (arvot_lkm[2] == 2)
        tulos |= 1 << KASI_ARVIO_KAKSIPARIA;

    if ((arvot[0] >= 2 || arvot[10] >= 2  /* A + J-K */
                    || arvot[11] >= 2 || arvot[12] >= 2))
        tulos |= 1 << KASI_ARVIO_KUVAPARI;

    kasi = MAX_KASI_LKM;
    for (i = 0; i < MAX_KASI_LKM; ++i)
        if ((tulos & (1 << i)) && kadet_kaytossa[i] >= 0
                               && kadet_kaytossa[i] < kasi)
            kasi = kadet_kaytossa[i];

    if (kasi == MAX_KASI_LKM)
        kasi = -1;
        
    voitto_kasi = kasi;
    return kasi >= 0;
}
