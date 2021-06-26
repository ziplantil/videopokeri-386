
#include <stdlib.h>
#include "POKERI.H"
#include "PAKKA.H"

kortti_t pakka[53];
int pakka_i;

unsigned long rng_seed;

void custom_srand(unsigned long u) {
    rng_seed = u;
}

#define CUSTOM_RAND_MAX 0x40000000UL

unsigned long custom_rand() {
    unsigned long s = rng_seed;
    s ^= s << 13;
    s ^= s >> 17;
    s ^= s << 5;
    rng_seed = s;
    return (unsigned long)(s * 0x75708a59UL) & (CUSTOM_RAND_MAX - 1);
}

void alusta_sekoitin(void) {
    unsigned long u /* intentionally uninitialized */;
    u = (u << 23) | (u >> 9);
    u ^= *(const volatile unsigned long*)0x46c;
    u = (u << 11) | (u >> 21);
    u ^= *(const volatile unsigned long*)0x41e;
    u = (u << 29) | (u >> 3);
    custom_srand(u);
}

int sekoitusluku(int n) { /* [0, n[ */
    unsigned long m, left = CUSTOM_RAND_MAX - (CUSTOM_RAND_MAX % n);
    int i = 0;
    do {
        m = custom_rand();
    } while (m >= left && i++ < 32);
    return (int)(m % n);
}

void alusta_pakka(void) {
    int i;
    for (i = 0; i < 52; ++i)
        pakka[i] = ((i / 13) << 4) | (i % 13);
    pakka[52] = K_JOKERI;
    pakka_i = 0;
}

void sekoita_pakka(int i) {
    kortti_t tmp;
    int j = sekoitusluku(i + 1);
    RANGE_CHECK(j, 0, i + 1);
    if (i != j)
        ARRAY_SWAP(pakka, i, j, tmp);
}

kortti_t jaa_kortti(void) {
    return pakka[pakka_i++];
}
