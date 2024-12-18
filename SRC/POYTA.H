
#ifndef _POYTA_H
#define _POYTA_H

#include "KORTTI.H"

extern int maksimipanos;
extern int paavoitto;
extern int jokeri_saatavilla;
extern int jokeri_minimipanos;

struct Korttikasi {
    kortti_t kortit[5];
};

struct Pokerikasi {
    const char *nimi;
    unsigned int kerroin;
    unsigned int kerroin_jokeri;
    const struct Korttikasi *esittely;
};

#define KASI_ARVIO_KUNINGASVARISUORA 0
#define KASI_ARVIO_VIITOSET 1
#define KASI_ARVIO_VARISUORA 2
#define KASI_ARVIO_NELOSET 3
#define KASI_ARVIO_TAYSKASI 4
#define KASI_ARVIO_VARI 5
#define KASI_ARVIO_SUORA 6
#define KASI_ARVIO_KOLMOSET 7
#define KASI_ARVIO_KAKSIPARIA 8
#define KASI_ARVIO_KUVAPARI 9
#define MAX_KASI_LKM 10

extern int voitto_kasi;
extern struct Pokerikasi kadet[MAX_KASI_LKM];
extern unsigned kasi_lkm;
extern kortti_t poyta[5];
extern kortti_t tuplakortti;

int alusta_kadet(int english, const int *kertoimet, const int *kertoimet_j);
int arvioi_voitto_kasi(void);

#endif
