
#ifndef _POYTA_H
#define _POYTA_H

#include "KORTTI.H"

extern int maksimipanos;
extern int paavoitto;
extern int jokeri_saatavilla;
extern int jokeri_minimipanos;

struct Pokerikasi {
    const char *nimi;
    unsigned int kerroin;
    unsigned int kerroin_jokeri;
};

#define KASI_LKM 8

extern int voitto_kasi;
extern struct Pokerikasi kadet[KASI_LKM];
extern kortti_t poyta[5];
extern kortti_t tuplakortti;

struct Korttikasi {
    kortti_t kortit[5];
};
extern struct Korttikasi kadet_esittely[KASI_LKM];

int arvioi_voitto_kasi(void);

#endif
