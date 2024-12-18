
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "POKERI.H"
#include "PAKKA.H"

kortti_t pakka[53];
static int pakka_i;

static unsigned long rng_seed;

static void custom_srand(unsigned long u) {
    rng_seed = u;
}

#define CUSTOM_RAND_MAX 0x10000000UL

static unsigned long custom_rand() {
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
    u += 816418833UL * *(const volatile unsigned long*)0x46c;
    u = (u << 11) | (u >> 21);
    u += 1611191243UL * *(const volatile unsigned long*)0x41e;
    u = (u << 29) | (u >> 3);
    u += 2058669331UL * (unsigned long)clock();
    u = (u << 15) | (u >> 17);
    custom_srand(u);
}

static int sekoitus_jaama = 0;
int sekoitusluku(int n) { /* [0, n[ */
    unsigned long m, left = CUSTOM_RAND_MAX - (CUSTOM_RAND_MAX % n);
    int i = 0;
    do {
        m = custom_rand();
    } while (m >= left && i++ < 16);
    if (m >= left) {
        int r = (int)((m + sekoitus_jaama) % n);
        sekoitus_jaama += left;
        return r;
    } else
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
    if (i != j) {
        tmp = pakka[i];
        pakka[i] = pakka[j];
        pakka[j] = tmp;
    }
}

kortti_t jaa_kortti(void) {
    assert(pakka_i <= 52);
    return pakka[pakka_i++];
}
